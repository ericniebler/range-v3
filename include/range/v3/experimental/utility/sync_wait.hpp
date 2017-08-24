// Range v3 library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// With thanks to Lewis Baker and Gor Nishanov
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_AWAIT_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_AWAIT_HPP

#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <mutex>
#include <condition_variable>
#include <experimental/coroutine>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace experimental
        {
            template<typename T,
                CONCEPT_REQUIRES_(CoAwaitable<T>())>
            concepts::CoAwaitable::value_t<T> sync_wait(T&& t)
            {
                struct _
                {
                    // In this awaitable's final_suspend, signal a condition variable. Then in
                    // the awaitable's loop() function, wait on the condition variable.
                    struct promise_type
                    {
                        std::mutex mtx_;
                        std::condition_variable cnd_;
                        bool done_{false};

                        std::experimental::suspend_never initial_suspend()
                        {
                            return {};
                        }
                        std::experimental::suspend_always final_suspend()
                        {
                            { std::lock_guard<std::mutex> g(mtx_); done_ = true; }
                            cnd_.notify_all();
                            return {};
                        }
                        _ get_return_object()
                        {
                            return _{*this};
                        }
                        void return_void()
                        {}
                        void unhandled_exception()
                        {}
                    };
                    promise_type* promise_;

                    explicit _(promise_type &p) noexcept
                      : promise_(&p)
                    {}
                    _(_ &&that) noexcept
                      : promise_(ranges::exchange(that.promise_, nullptr))
                    {}
                    ~_()
                    {
                        if(promise_)
                            // Needed because final_suspend returns suspend_always:
                            std::experimental::coroutine_handle<promise_type>::
                                from_promise(*promise_).destroy();
                    }
                    void wait()
                    {
                        std::unique_lock<std::mutex> lock(promise_->mtx_);
                        promise_->cnd_.wait(lock, [this] { return promise_->done_; });
                    }
                };

                [&]() -> _ { (void)co_await t; }().wait();
                return as_awaitable(std::forward<T>(t)).await_resume();
            }
        }
    }
}

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_AWAIT_HPP
