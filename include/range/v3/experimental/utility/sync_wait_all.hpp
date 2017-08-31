// Range v3 library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// With thanks to Lewis Baker
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_WAIT_ALL_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_WAIT_ALL_HPP

#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <mutex>
#include <exception>
#include <condition_variable>
#include <experimental/coroutine>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/invoke.hpp> // for reference_wrapper

namespace ranges
{
    inline namespace v3
    {
        namespace experimental
        {
            struct void_
            {};

            /// \cond
            namespace detail
            {
                struct sync_wait_all_promise_base
                {
                    std::exception_ptr eptr_;
                    void unhandled_exception() noexcept
                    {
                        eptr_ = std::current_exception();
                    }
                    void_ get() const
                    {
                        if(eptr_)
                        {
                            std::rethrow_exception(eptr_);
                        }
                        return {};
                    }
                };
                struct sync_wait_all_void_promise
                  : sync_wait_all_promise_base
                {
                    void return_void() const noexcept
                    {}
                };
                template<typename T>
                struct sync_wait_all_value_promise
                  : sync_wait_all_promise_base
                {
                    using value_type = meta::if_<std::is_reference<T>, reference_wrapper<T>, T>;
                    alignas(value_type) char buffer_[sizeof(value_type)];
                    template<typename U>
                    void return_value(U &&u)
                    {
                        ::new((void*)&buffer_) value_type(static_cast<U &&>(u));
                    }
                    T &get()
                    {
                        this->sync_wait_all_promise_base::get();
                        return *static_cast<value_type *>((void*)&buffer_);
                    }
                };

                template<typename T>
                using co_result_not_void_t =
                    meta::if_<std::is_void<co_result_t<T>>, void_, co_result_t<T>>;

                template<typename... Ts>
                struct wait_all_outer
                {
                    // In this awaitable's final_suspend, signal a condition variable. Then in
                    // the awaitable's wait() function, wait on the condition variable.
                    struct promise_type
                      : detail::sync_wait_all_value_promise<std::tuple<co_result_not_void_t<Ts>...>>
                    {
                        std::mutex mtx_;
                        std::condition_variable cnd_;
                        std::size_t count_{sizeof...(Ts)};

                        std::experimental::suspend_never initial_suspend() const noexcept
                        {
                            return {};
                        }
                        std::experimental::suspend_always final_suspend() const noexcept
                        {
                            return {};
                        }
                        wait_all_outer get_return_object() noexcept
                        {
                            return wait_all_outer{*this};
                        }
                    };
                    std::experimental::coroutine_handle<promise_type> coro_;

                    explicit wait_all_outer(promise_type &p) noexcept
                      : coro_(std::experimental::coroutine_handle<promise_type>::from_promise(p))
                    {}
                    wait_all_outer(wait_all_outer &&that) noexcept
                      : coro_(ranges::exchange(that.coro_, nullptr))
                    {}
                    ~wait_all_outer()
                    {
                        if(coro_)
                            coro_.destroy();
                    }
                    std::tuple<co_result_not_void_t<Ts>...> wait()
                    {
                        auto &promise = coro_.promise();
                        std::unique_lock<std::mutex> lock(promise.mtx_);
                        promise.cnd_.wait(lock, [&promise] { return promise.count_ == 0; });
                        return promise.get();
                    }
                };

                template<typename T, typename... Ts>
                struct wait_all_inner
                {
                    struct promise_type
                      : meta::if_<
                            std::is_void<co_result_t<T>>,
                            sync_wait_all_void_promise,
                            sync_wait_all_value_promise<co_result_t<T>>>
                    {
                        using outer_promise_type = typename wait_all_outer<Ts...>::promise_type;

                        struct final_suspend_result : std::experimental::suspend_always
                        {
                            outer_promise_type *outer_;
                            final_suspend_result(outer_promise_type *outer) noexcept
                              : outer_(outer)
                            {}
                            void await_suspend(std::experimental::coroutine_handle<>) const noexcept
                            {
                                { std::lock_guard<std::mutex> g(outer_->mtx_); --outer_->count_; }
                                outer_->cnd_.notify_all();
                            }
                        };

                        outer_promise_type *outer_;

                        std::experimental::suspend_always initial_suspend() const noexcept
                        {
                            return {};
                        }
                        final_suspend_result final_suspend() const noexcept
                        {
                            return {outer_};
                        }
                        wait_all_inner get_return_object() noexcept
                        {
                            return wait_all_inner{*this};
                        }
                    };

                    std::experimental::coroutine_handle<promise_type> coro_;

                    explicit wait_all_inner(promise_type &p) noexcept
                      : coro_(std::experimental::coroutine_handle<promise_type>::from_promise(p))
                    {}
                    wait_all_inner(wait_all_inner &&that) noexcept
                      : coro_(ranges::exchange(that.coro_, nullptr))
                    {}
                    ~wait_all_inner()
                    {
                        if(coro_)
                            coro_.destroy();
                    }
                    bool await_ready() const noexcept
                    {
                        RANGES_EXPECT(!coro_.done());
                        return false;
                    }
                    bool await_suspend(std::experimental::coroutine_handle<
                        typename wait_all_outer<Ts...>::promise_type> waiter) noexcept
                    {
                        coro_.promise().outer_ = &waiter.promise();
                        coro_.resume();
                        return false; // don't suspend, resume the waiting coroutine
                    }
                    co_result_not_void_t<T> await_resume() const
                    {
                        return coro_.promise().get();
                    }
                };
            } // namespace detail
            /// \endcond

            // template <CoAwaitable ...Ts>
            template<typename ...Ts,
                CONCEPT_REQUIRES_(meta::and_<CoAwaitable<Ts>...>())>
            std::tuple<detail::co_result_not_void_t<Ts>...> sync_wait_all(Ts &&...ts)
            {
                return [](Ts &...us) -> detail::wait_all_outer<Ts...> {
                    co_return std::tuple<detail::co_result_not_void_t<Ts>...>{
                        co_await [](Ts &v) -> detail::wait_all_inner<Ts, Ts...> {
                            co_return co_await v;
                        }(us)...
                    };
                }(ts...).wait();
            }
        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_AWAIT_HPP
