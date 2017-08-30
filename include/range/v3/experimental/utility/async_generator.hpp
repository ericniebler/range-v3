// Range v3 library
//
// Copyright 2017 Lewis Baker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Copyright 2017 Eric Niebler
//
// Dual-licensed under the Boost Software License:
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_ASYNC_GENERATOR_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_ASYNC_GENERATOR_HPP

#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <atomic>
#include <exception>
#include <experimental/coroutine>
#include <functional>
#include <iterator>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp>
#include <type_traits>

namespace ranges
{
    inline namespace v3
    {
        namespace experimental
        {
            template<typename T>
            struct async_generator;

            /// \cond
            namespace detail
            {
                template<typename T>
                struct async_generator_iterator;
                struct async_generator_yield_operation;
                struct async_generator_advance_operation;

                struct async_generator_promise_base
                {
                private:
                    friend async_generator_yield_operation;
                    friend async_generator_advance_operation;

                    // State transition diagram
                    //   VNRCA - value_not_ready_consumer_active
                    //   VNRCS - value_not_ready_consumer_suspended
                    //   VRPA  - value_ready_consumer_active
                    //   VRPS  - value_ready_consumer_suspended
                    //
                    //       A         +---  VNRCA --[C]--> VNRCS   yield_value()
                    //       |         |     |  A           |  A       |   .
                    //       |        [C]   [P] |          [P] |       |   .
                    //       |         |     | [C]          | [C]      |   .
                    //       |         |     V  |           V  |       |   .
                    //  operator++/    |     VRPS <--[P]--- VRPA       V   |
                    //  begin()        |      |              |             |
                    //                 |     [C]            [C]            |
                    //                 |      +----+     +---+             |
                    //                 |           |     |                 |
                    //                 |           V     V                 V
                    //                 +--------> cancelled         ~async_generator()
                    //
                    // [C] - Consumer performs this transition
                    // [P] - Producer performs this transition
                    enum struct state
                    {
                        value_not_ready_consumer_active,
                        value_not_ready_consumer_suspended,
                        value_ready_producer_active,
                        value_ready_producer_suspended,
                        cancelled
                    };

                    std::atomic<state> state_;
                    std::exception_ptr exception_;
                    std::experimental::coroutine_handle<> consumerCoroutine_;

                protected:
                    async_generator_yield_operation internal_yield_value() noexcept;
                    void *currentValue_;

                public:
                    async_generator_promise_base() noexcept
                      : state_(state::value_ready_producer_suspended)
                      , exception_(nullptr)
                    {
                        // Other variables left intentionally uninitialised as they're
                        // only referenced in certain states by which time they should
                        // have been initialised.
                    }

                    async_generator_promise_base(async_generator_promise_base const &other) =
                        delete;
                    async_generator_promise_base &operator=(
                        async_generator_promise_base const &other) = delete;

                    std::experimental::suspend_always initial_suspend() const noexcept
                    {
                        return {};
                    }

                    async_generator_yield_operation final_suspend() noexcept;

                    void unhandled_exception() noexcept
                    {
                        // Don't bother capturing the exception if we have been cancelled
                        // as there is no consumer that will see it.
                        if(state_.load(std::memory_order_relaxed) != state::cancelled)
                        {
                            exception_ = std::current_exception();
                        }
                    }

                    void return_void() noexcept {}

                    /// Query if the generator has reached the end of the sequence.
                    ///
                    /// Only valid to call after resuming from an awaited advance
                    /// operation. i.e. Either a begin() or iterator::operator++()
                    /// operation.
                    bool finished() const noexcept
                    {
                        return currentValue_ == nullptr;
                    }

                    void rethrow_if_unhandled_exception()
                    {
                        if(exception_)
                        {
                            std::rethrow_exception(std::move(exception_));
                        }
                    }

                    /// Request that the generator cancel generation of new items.
                    ///
                    /// \return
                    /// Returns true if the request was completed synchronously and the
                    /// associated producer coroutine is now available to be destroyed. In
                    /// which case the caller is expected to call destroy() on the
                    /// coroutine_handle. Returns false if the producer coroutine was not
                    /// at a suitable suspend-point. The coroutine will be destroyed when
                    /// it next reaches a co_yield or co_return statement.
                    bool request_cancellation() noexcept
                    {
                        const auto previousState =
                            state_.exchange(state::cancelled, std::memory_order_acq_rel);

                        // Not valid to destroy async_generator<T> object if consumer
                        // coroutine still suspended in a co_await for next item.
                        RANGES_EXPECT(previousState !=
                                      state::value_not_ready_consumer_suspended);

                        // A coroutine should only ever be cancelled once, from the
                        // destructor of the owning async_generator<T> object.
                        RANGES_EXPECT(previousState != state::cancelled);
                        return previousState == state::value_ready_producer_suspended;
                    }
                };

                struct async_generator_yield_operation final
                {
                private:
                    using state = async_generator_promise_base::state;
                    async_generator_promise_base &promise_;
                    state initialState_;

                public:
                    async_generator_yield_operation(async_generator_promise_base &promise,
                                                    state initialState) noexcept
                      : promise_(promise)
                      , initialState_(initialState)
                    {
                    }

                    bool await_ready() const noexcept
                    {
                        return initialState_ == state::value_not_ready_consumer_suspended;
                    }

                    bool await_suspend(std::experimental::coroutine_handle<> producer) noexcept;

                    void await_resume() noexcept {}
                };

                inline async_generator_yield_operation
                async_generator_promise_base::final_suspend() noexcept
                {
                    currentValue_ = nullptr;
                    return internal_yield_value();
                }

                inline async_generator_yield_operation
                async_generator_promise_base::internal_yield_value() noexcept
                {
                    state currentState = state_.load(std::memory_order_acquire);
                    RANGES_EXPECT(currentState != state::value_ready_producer_active);
                    RANGES_EXPECT(currentState != state::value_ready_producer_suspended);

                    if(currentState == state::value_not_ready_consumer_suspended)
                    {
                        // Only need relaxed memory order since we're resuming the
                        // consumer on the same thread.
                        state_.store(state::value_ready_producer_active,
                                     std::memory_order_relaxed);

                        // Resume the consumer.
                        // It might ask for another value before returning, in which case
                        // it'll transition to value_not_ready_consumer_suspended and we
                        // can return from yield_value without suspending, otherwise we
                        // should try to suspend the producer in which case the consumer
                        // will wake us up again when it wants the next value.
                        consumerCoroutine_.resume();

                        // Need to use acquire semantics here since it's possible that the
                        // consumer might have asked for the next value on a different
                        // thread which executed concurrently with the call to
                        // consumerCoro_ on the current thread above.
                        currentState = state_.load(std::memory_order_acquire);
                    }

                    return async_generator_yield_operation{*this, currentState};
                }

                inline bool async_generator_yield_operation::await_suspend(
                    std::experimental::coroutine_handle<> producer) noexcept
                {
                    state currentState = initialState_;
                    if(currentState == state::value_not_ready_consumer_active)
                    {
                        bool producerSuspended = promise_.state_.compare_exchange_strong(
                            currentState,
                            state::value_ready_producer_suspended,
                            std::memory_order_release,
                            std::memory_order_acquire);
                        if(producerSuspended)
                        {
                            return true;
                        }

                        if(currentState == state::value_not_ready_consumer_suspended)
                        {
                            // Can get away with using relaxed memory semantics here since
                            // we're resuming the consumer on the current thread.
                            promise_.state_.store(state::value_ready_producer_active,
                                                  std::memory_order_relaxed);

                            promise_.consumerCoroutine_.resume();

                            // The consumer might have asked for another value before
                            // returning, in which case it'll transition to
                            // value_not_ready_consumer_suspended and we can return without
                            // suspending, otherwise we should try to suspend the producer,
                            // in which case the consumer will wake us up again when it
                            // wants the next value.
                            //
                            // Need to use acquire semantics here since it's possible that
                            // the consumer might have asked for the next value on a
                            // different thread which executed concurrently with the call
                            // to consumerCoro_.resume() above.
                            currentState = promise_.state_.load(std::memory_order_acquire);
                            if(currentState == state::value_not_ready_consumer_suspended)
                            {
                                return false;
                            }
                        }
                    }

                    // By this point the consumer has been resumed if required and is now
                    // active.

                    if(currentState == state::value_ready_producer_active)
                    {
                        // Try to suspend the producer.
                        // If we failed to suspend then it's either because the consumer
                        // destructed, transitioning the state to cancelled, or requested
                        // the next item, transitioning the state to
                        // value_not_ready_consumer_suspended.
                        const bool suspendedProducer = promise_.state_.compare_exchange_strong(
                            currentState,
                            state::value_ready_producer_suspended,
                            std::memory_order_release,
                            std::memory_order_acquire);
                        if(suspendedProducer)
                        {
                            return true;
                        }

                        if(currentState == state::value_not_ready_consumer_suspended)
                        {
                            // Consumer has asked for the next value.
                            return false;
                        }
                    }

                    RANGES_EXPECT(currentState == state::cancelled);

                    // async_generator object has been destroyed and we're now at a
                    // co_yield/co_return suspension point so we can just destroy
                    // the coroutine.
                    producer.destroy();
                    return true;
                }

                struct async_generator_advance_operation
                {
                private:
                    using state = async_generator_promise_base::state;
                    state initialState_;

                protected:
                    async_generator_promise_base *promise_;
                    std::experimental::coroutine_handle<> producerCoroutine_;
                    async_generator_advance_operation(std::nullptr_t) noexcept
                      : promise_(nullptr)
                      , producerCoroutine_(nullptr)
                    {
                    }

                    async_generator_advance_operation(
                        async_generator_promise_base &promise,
                        std::experimental::coroutine_handle<> producerCoroutine) noexcept
                      : promise_(std::addressof(promise))
                      , producerCoroutine_(producerCoroutine)
                    {
                        state initialState = promise.state_.load(std::memory_order_acquire);
                        if(initialState == state::value_ready_producer_suspended)
                        {
                            // Can use relaxed memory order here as we will be resuming the
                            // producer on the same thread.
                            promise.state_.store(state::value_not_ready_consumer_active,
                                                 std::memory_order_relaxed);

                            producerCoroutine.resume();

                            // Need to use acquire memory order here since it's possible
                            // that the coroutine may have transferred execution to another
                            // thread and completed on that other thread before the call to
                            // resume() returns.
                            initialState = promise.state_.load(std::memory_order_acquire);
                        }

                        initialState_ = initialState;
                    }

                public:
                    bool await_ready() const noexcept
                    {
                        return initialState_ == state::value_ready_producer_suspended;
                    }

                    bool await_suspend(
                        std::experimental::coroutine_handle<> consumerCoroutine) noexcept
                    {
                        promise_->consumerCoroutine_ = consumerCoroutine;

                        auto currentState = initialState_;
                        if(currentState == state::value_ready_producer_active)
                        {
                            // A potential race between whether consumer or producer
                            // coroutine suspends first. Resolve the race using a
                            // compare-exchange.
                            if(promise_->state_.compare_exchange_strong(
                                   currentState,
                                   state::value_not_ready_consumer_suspended,
                                   std::memory_order_release,
                                   std::memory_order_acquire))
                            {
                                return true;
                            }

                            RANGES_EXPECT(currentState ==
                                          state::value_ready_producer_suspended);

                            promise_->state_.store(state::value_not_ready_consumer_active,
                                                   std::memory_order_relaxed);

                            producerCoroutine_.resume();

                            currentState = promise_->state_.load(std::memory_order_acquire);
                            if(currentState == state::value_ready_producer_suspended)
                            {
                                // Producer coroutine produced a value synchronously.
                                return false;
                            }
                        }

                        RANGES_EXPECT(currentState == state::value_not_ready_consumer_active);

                        // Try to suspend consumer coroutine, transitioning to
                        // value_not_ready_consumer_suspended. This could be racing with
                        // producer making the next value available and suspending
                        // (transition to value_ready_producer_suspended) so we use
                        // compare_exchange to decide who wins the race. If
                        // compare_exchange succeeds then consumer suspended (and we return
                        // true). If it fails then producer yielded next value and
                        // suspended and we can return synchronously without suspended (ie.
                        // return false).
                        return promise_->state_.compare_exchange_strong(
                            currentState,
                            state::value_not_ready_consumer_suspended,
                            std::memory_order_release,
                            std::memory_order_acquire);
                    }
                };

                template<typename T>
                struct async_generator_promise final : async_generator_promise_base
                {
                private:
                    using value_type = meta::_t<std::remove_reference<T>>;

                public:
                    async_generator_promise() noexcept = default;

                    async_generator<T> get_return_object() noexcept;

                    async_generator_yield_operation yield_value(value_type &value) noexcept
                    {
                        currentValue_ = std::addressof(value);
                        return internal_yield_value();
                    }

                    async_generator_yield_operation yield_value(value_type &&value) noexcept
                    {
                        return yield_value(value);
                    }

                    T &value() const noexcept
                    {
                        return *static_cast<T *>(currentValue_);
                    }
                };

                template<typename T>
                struct async_generator_promise<T &&> final : async_generator_promise_base
                {
                    async_generator_promise() noexcept = default;

                    async_generator<T> get_return_object() noexcept;

                    async_generator_yield_operation yield_value(T &&value) noexcept
                    {
                        currentValue_ = std::addressof(value);
                        return internal_yield_value();
                    }

                    T &&value() const noexcept
                    {
                        return std::move(*static_cast<T *>(currentValue_));
                    }
                };

                template<typename T>
                struct async_generator_preincrement_operation final
                  : async_generator_advance_operation
                {
                    async_generator_preincrement_operation(
                        async_generator_iterator<T> &iterator) noexcept
                      : async_generator_advance_operation(iterator.coroutine_.promise(),
                                                          iterator.coroutine_)
                      , iterator_(iterator)
                    {
                    }

                    async_generator_iterator<T> &await_resume();

                private:
                    async_generator_iterator<T> &iterator_;
                };

                template<typename T>
                struct async_generator_postincrement_operation final
                  : async_generator_advance_operation
                {
                    async_generator_postincrement_operation(
                        async_generator_iterator<T> &iterator) noexcept
                      : async_generator_advance_operation(iterator.coroutine_.promise(),
                                                          iterator.coroutine_)
                      , iterator_(iterator)
                    {
                    }

                    void await_resume();

                private:
                    async_generator_iterator<T> &iterator_;
                };

                template<typename T>
                struct async_generator_iterator final
                {
                private:
                    friend async_generator_preincrement_operation<T>;
                    friend async_generator_postincrement_operation<T>;
                    using promise_type = async_generator_promise<T>;
                    using handle_type = std::experimental::coroutine_handle<promise_type>;
                    handle_type coroutine_;

                public:
                    using difference_type = std::ptrdiff_t;
                    using value_type = uncvref_t<T>;

                    async_generator_iterator(std::nullptr_t) noexcept
                      : coroutine_(nullptr)
                    {
                    }

                    async_generator_iterator(handle_type coroutine) noexcept
                      : coroutine_(coroutine)
                    {
                    }

                    async_generator_preincrement_operation<T> operator++() noexcept
                    {
                        return async_generator_preincrement_operation<T>{*this};
                    }

                    async_generator_postincrement_operation<T> operator++(int)noexcept
                    {
                        return async_generator_postincrement_operation<T>{*this};
                    }

                    T operator*() const noexcept
                    {
                        return coroutine_.promise().value();
                    }

                    bool operator==(async_generator_iterator const &other) const noexcept
                    {
                        return coroutine_ == other.coroutine_;
                    }

                    bool operator!=(async_generator_iterator const &other) const noexcept
                    {
                        return !(*this == other);
                    }
                };

                template<typename T>
                async_generator_iterator<T>
                    &async_generator_preincrement_operation<T>::await_resume()
                {
                    if(promise_->finished())
                    {
                        // Update iterator to end()
                        iterator_ = async_generator_iterator<T>{nullptr};
                        promise_->rethrow_if_unhandled_exception();
                    }

                    return iterator_;
                }

                template<typename T>
                void async_generator_postincrement_operation<T>::await_resume()
                {
                    if(promise_->finished())
                    {
                        // Update iterator to end()
                        iterator_ = async_generator_iterator<T>{nullptr};
                        promise_->rethrow_if_unhandled_exception();
                    }
                }

                template<typename T>
                struct async_generator_begin_operation final : async_generator_advance_operation
                {
                private:
                    using promise_type = async_generator_promise<T>;
                    using handle_type = std::experimental::coroutine_handle<promise_type>;

                public:
                    async_generator_begin_operation(std::nullptr_t) noexcept
                      : async_generator_advance_operation(nullptr)
                    {
                    }

                    async_generator_begin_operation(handle_type producerCoroutine) noexcept
                      : async_generator_advance_operation(producerCoroutine.promise(),
                                                          producerCoroutine)
                    {
                    }

                    bool await_ready() const noexcept
                    {
                        return promise_ == nullptr ||
                               async_generator_advance_operation::await_ready();
                    }

                    async_generator_iterator<T> await_resume()
                    {
                        if(promise_ == nullptr)
                        {
                            // Called begin() on the empty generator.
                            return async_generator_iterator<T>{nullptr};
                        }
                        else if(promise_->finished())
                        {
                            // Completed without yielding any values.
                            promise_->rethrow_if_unhandled_exception();
                            return async_generator_iterator<T>{nullptr};
                        }

                        return async_generator_iterator<T>{
                            handle_type::from_promise(*static_cast<promise_type *>(promise_))};
                    }
                };
            }

            template<typename T>
            struct async_generator
            {
            public:
                using promise_type = detail::async_generator_promise<T>;
                using iterator = detail::async_generator_iterator<T>;

                async_generator() noexcept
                  : coroutine_(nullptr)
                {
                }

                explicit async_generator(promise_type &promise) noexcept
                  : coroutine_(std::experimental::coroutine_handle<promise_type>::from_promise(
                        promise))
                {
                }

                async_generator(async_generator &&other) noexcept
                  : coroutine_(other.coroutine_)
                {
                    other.coroutine_ = nullptr;
                }

                ~async_generator()
                {
                    if(coroutine_)
                    {
                        if(coroutine_.promise().request_cancellation())
                        {
                            coroutine_.destroy();
                        }
                    }
                }

                async_generator &operator=(async_generator &&other) noexcept
                {
                    async_generator(std::move(other)).swap(*this);
                    return *this;
                }

                async_generator(async_generator const &) = delete;
                async_generator &operator=(async_generator const &) = delete;

                detail::async_generator_begin_operation<T> begin() noexcept
                {
                    if(!coroutine_)
                    {
                        return detail::async_generator_begin_operation<T>{nullptr};
                    }

                    return detail::async_generator_begin_operation<T>{coroutine_};
                }

                iterator end() noexcept
                {
                    return iterator{nullptr};
                }

                void swap(async_generator &other) noexcept
                {
                    ranges::swap(coroutine_, other.coroutine_);
                }

            private:
                std::experimental::coroutine_handle<promise_type> coroutine_;
            };

            template<typename T>
            void swap(async_generator<T> &a, async_generator<T> &b) noexcept
            {
                a.swap(b);
            }

            /// \cond
            namespace detail
            {
                template<typename T>
                async_generator<T> async_generator_promise<T>::get_return_object() noexcept
                {
                    return async_generator<T>{*this};
                }
            } // namespace detail
            /// \endcond
        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_GENERATOR_HPP
