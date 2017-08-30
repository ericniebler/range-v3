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
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_WAIT_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_WAIT_HPP

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
            /// \cond
            namespace detail
            {
                struct sync_wait_promise_base
                {
                    std::exception_ptr eptr_;
                    void unhandled_exception() noexcept
                    {
                        eptr_ = std::current_exception();
                    }
                    void get() const
                    {
                        if(eptr_)
                        {
                            std::rethrow_exception(eptr_);
                        }
                    }
                };
                struct sync_wait_void_promise : sync_wait_promise_base
                {
                    void return_void() const noexcept
                    {}
                };
                template<typename T>
                struct sync_wait_value_promise : sync_wait_promise_base
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
                        this->sync_wait_promise_base::get();
                        return *static_cast<value_type *>((void*)&buffer_);
                    }
                };
            } // namespace detail
            /// \endcond

            template<typename T,
                CONCEPT_REQUIRES_(CoAwaitable<T>())>
            co_await_resume_t<T> sync_wait(T &&t)
            {
                struct _
                {
                    // In this awaitable's final_suspend, signal a condition variable. Then in
                    // the awaitable's wait() function, wait on the condition variable.
                    struct promise_type
                      : meta::if_<
                            std::is_void<co_await_resume_t<T>>,
                            detail::sync_wait_void_promise,
                            detail::sync_wait_value_promise<co_await_resume_t<T>>>
                    {
                        std::mutex mtx_;
                        std::condition_variable cnd_;
                        bool done_{false};

                        std::experimental::suspend_never initial_suspend() const noexcept
                        {
                            return {};
                        }
                        std::experimental::suspend_always final_suspend() noexcept
                        {
                            { std::lock_guard<std::mutex> g(mtx_); done_ = true; }
                            cnd_.notify_all();
                            return {};
                        }
                        auto get_return_object() noexcept -> _
                        {
                            return _{*this};
                        }
                    };
                    promise_type *promise_;

                    explicit _(promise_type &p) noexcept
                      : promise_(&p)
                    {}
                    _(_ &&that) noexcept
                      : promise_(ranges::exchange(that.promise_, nullptr))
                    {}
                    ~_()
                    {
                        if(promise_)
                        {
                            // Needed because final_suspend returns suspend_always:
                            std::experimental::coroutine_handle<promise_type>::
                                from_promise(*promise_).destroy();
                        }
                    }
                    co_await_resume_t<T> wait()
                    {
                        std::unique_lock<std::mutex> lock(promise_->mtx_);
                        promise_->cnd_.wait(lock, [this] { return promise_->done_; });
                        return promise_->get();
                    }
                };

                return [](T &t) -> _ { co_return co_await t; }(t).wait();
            }
        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_AWAIT_HPP
