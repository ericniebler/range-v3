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

// #include <cstdio>
// #include <typeinfo>

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
                        // std::printf("In return_value (%s)\n", typeid(T).name());
                        // std::fflush(stdout);
                        ::new((void*)&buffer_) value_type(static_cast<U &&>(u));
                    }
                    T &get()
                    {
                        this->sync_wait_all_promise_base::get();
                        return *static_cast<value_type *>((void*)&buffer_);
                    }
                };

                template<typename T>
                using wait_all_element_t =
                    meta::if_<std::is_void<co_await_resume_t<T>>, void_, co_await_resume_t<T>>;

                template<typename... Ts>
                struct wait_all_outer
                {
                    // In this awaitable's final_suspend, signal a condition variable. Then in
                    // the awaitable's wait() function, wait on the condition variable.
                    struct promise_type
                      : detail::sync_wait_all_value_promise<std::tuple<wait_all_element_t<Ts>...>>
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
                    } *promise_;

                    explicit wait_all_outer(promise_type &p) noexcept
                      : promise_(&p)
                    {}
                    wait_all_outer(wait_all_outer &&that) noexcept
                      : promise_(ranges::exchange(that.promise_, nullptr))
                    {}
                    ~wait_all_outer()
                    {
                        if(promise_)
                        {
                            std::experimental::coroutine_handle<promise_type>::
                                from_promise(*promise_).destroy();
                        }
                    }
                    std::tuple<wait_all_element_t<Ts>...> wait()
                    {
                        std::unique_lock<std::mutex> lock(promise_->mtx_);
                        promise_->cnd_.wait(lock, [this] { return promise_->count_ == 0; });
                        return promise_->get();
                    }
                };

                template<typename T, typename... Ts>
                struct wait_all_inner
                {
                    struct promise_type
                      : meta::if_<
                            std::is_void<co_await_resume_t<T>>,
                            sync_wait_all_void_promise,
                            sync_wait_all_value_promise<co_await_resume_t<T>>>
                    {
                        typename wait_all_outer<Ts...>::promise_type *outer_;

                        std::experimental::suspend_always initial_suspend() const noexcept
                        {
                            // std::printf("wait_all_inner::promise_type::initial_suspend\n");
                            // std::fflush(stdout);
                            return {};
                        }
                        std::experimental::suspend_always final_suspend() noexcept
                        {
                            // std::printf("wait_all_inner::promise_type::final_suspend\n");
                            // std::fflush(stdout);
                            { std::lock_guard<std::mutex> g(outer_->mtx_); --outer_->count_; }
                            outer_->cnd_.notify_all();
                            return {};
                        }
                        wait_all_inner get_return_object() noexcept
                        {
                            // std::printf("wait_all_inner::promise_type::get_return_object\n");
                            // std::fflush(stdout);
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
                        // std::printf("wait_all_inner::await_ready\n");
                        // std::fflush(stdout);
                        RANGES_EXPECT(!coro_.done());
                        return false;
                    }
                    bool await_suspend(std::experimental::coroutine_handle<
                        typename wait_all_outer<Ts...>::promise_type> waiter) noexcept
                    {
                        // std::printf("wait_all_inner::await_suspend\n");
                        // std::fflush(stdout);
                        coro_.promise().outer_ = &waiter.promise();
                        coro_.resume();
                        return false; // Why? Not sure yet.
                    }
                    wait_all_element_t<T> await_resume() const
                    {
                        // std::printf("wait_all_inner::await_resume\n");
                        // std::fflush(stdout);
                        return coro_.promise().get();
                    }
                };
            } // namespace detail
            /// \endcond

            template<typename... Ts,
                CONCEPT_REQUIRES_(meta::and_<CoAwaitable<Ts>...>())>
            std::tuple<detail::wait_all_element_t<Ts>...> sync_wait_all(Ts &&...ts)
            {
                return [](Ts &...us) -> detail::wait_all_outer<Ts...> {
                    co_return std::tuple<detail::wait_all_element_t<Ts>...>{
                        (co_await ([](Ts &v) -> detail::wait_all_inner<Ts, Ts...> {
                            co_return co_await v;
                        }(us)))...
                    };
                }(ts...).wait();
            }
        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_AWAIT_HPP
