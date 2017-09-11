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
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_WHEN_ALL_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_WHEN_ALL_HPP

#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <atomic>
#include <exception>
#include <experimental/coroutine>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/invoke.hpp> // for reference_wrapper
#include <range/v3/experimental/utility/sync_wait.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace experimental
        {
            /// \cond
            namespace detail
            {
                struct when_all_promise_base
                {
                    std::exception_ptr eptr_;
                    void unhandled_exception() noexcept
                    {
                        eptr_ = std::current_exception();
                    }
                    monostate get() const
                    {
                        if(eptr_)
                        {
                            std::rethrow_exception(eptr_);
                        }
                        return {};
                    }
                };
                struct when_all_void_promise
                  : when_all_promise_base
                {
                    void return_void() const noexcept
                    {}
                };
                template<typename T>
                struct when_all_value_promise
                  : when_all_promise_base
                {
                    using value_type = meta::if_<std::is_reference<T>, reference_wrapper<T>, T>;
                    bool is_set_{false};
                    alignas(value_type) char buffer_[sizeof(value_type)];
                    template<typename U>
                    void return_value(U &&u)
                    {
                        RANGES_EXPECT(!is_set_);
                        ::new((void*)&buffer_) value_type(static_cast<U &&>(u));
                        is_set_ = true;
                    }
                    ~when_all_value_promise()
                    {
                        if(is_set_)
                            static_cast<value_type *>((void*)&buffer_)->~value_type();
                    }
                    T &&get()
                    {
                        this->when_all_promise_base::get();
                        return static_cast<T &&>(*static_cast<value_type *>((void*)&buffer_));
                    }
                };

                template<typename T>
                using co_result_not_void_t =
                    meta::if_<std::is_void<co_result_t<T>>, monostate, co_result_t<T>>;

                template<typename... Ts>
                struct when_all_awaitable
                {
                    // In when_all_inner's final_suspend, signal a condition variable. Then in
                    // this awaitable's wait() function, wait on the condition variable.
                    struct promise_type
                      : detail::when_all_value_promise<std::tuple<co_result_not_void_t<Ts>...>>
                    {
                        std::size_t started_{sizeof...(Ts)};
                        std::atomic<std::size_t> stopped_{sizeof...(Ts)};

                        std::experimental::suspend_never initial_suspend() const noexcept
                        {
                            return {};
                        }
                        std::experimental::suspend_always final_suspend() const noexcept
                        {
                            return {};
                        }
                        when_all_awaitable get_return_object() noexcept
                        {
                            return when_all_awaitable{*this};
                        }
                    };
                    using coro_handle_t = std::experimental::coroutine_handle<promise_type>;
                    coro_handle_t coro_;

                    explicit when_all_awaitable(promise_type &p) noexcept
                      : coro_(coro_handle_t::from_promise(p))
                    {}
                    when_all_awaitable(when_all_awaitable &&that) noexcept
                      : coro_(ranges::exchange(that.coro_, nullptr))
                    {}
                    ~when_all_awaitable()
                    {
                        if(coro_)
                            coro_.destroy();
                    }
                    bool await_ready() const noexcept
                    {
                        return coro_.done();
                    }
                    void await_suspend(std::experimental::coroutine_handle<>) noexcept
                    {
                        coro_.resume();
                    }
                    std::tuple<co_result_not_void_t<Ts>...> await_resume()
                    {
                        auto &promise = coro_.promise();
                        RANGES_EXPECT(promise.stopped_ == 0);
                        return promise.get();
                    }
                };

                template<typename T, typename... Ts>
                struct when_all_inner
                {
                    using outer_promise_type_t = typename when_all_awaitable<Ts...>::promise_type;
                    using outer_coro_handle_t = std::experimental::coroutine_handle<outer_promise_type_t>;

                    struct promise_type
                      : meta::if_<
                            std::is_void<co_result_t<T>>,
                            when_all_void_promise,
                            when_all_value_promise<co_result_t<T>>>
                    {
                        struct final_suspend_result : std::experimental::suspend_always
                        {
                            outer_coro_handle_t outer_coro_;
                            final_suspend_result(outer_coro_handle_t outer) noexcept
                              : outer_coro_(outer)
                            {}
                            void await_suspend(std::experimental::coroutine_handle<>) noexcept
                            {
                                // If we're the last inner coro to finish, restart the outer coro
                                if (0 == --outer_coro_.promise().stopped_)
                                    outer_coro_.resume();
                            }
                        };

                        outer_coro_handle_t outer_coro_;

                        std::experimental::suspend_always initial_suspend() const noexcept
                        {
                            return {};
                        }
                        final_suspend_result final_suspend() const noexcept
                        {
                            return {outer_coro_};
                        }
                        when_all_inner get_return_object() noexcept
                        {
                            return when_all_inner{*this};
                        }
                    };

                    using coro_handle = std::experimental::coroutine_handle<promise_type>;
                    coro_handle coro_;

                    explicit when_all_inner(promise_type &p) noexcept
                      : coro_(coro_handle::from_promise(p))
                    {}
                    when_all_inner(when_all_inner &&that) noexcept
                      : coro_(ranges::exchange(that.coro_, nullptr))
                    {}
                    ~when_all_inner()
                    {
                        if(coro_)
                            coro_.destroy();
                    }
                    bool await_ready() const noexcept
                    {
                        RANGES_EXPECT(!coro_.done());
                        return false;
                    }
                    bool await_suspend(outer_coro_handle_t outer_coro) noexcept
                    {
                        coro_.promise().outer_coro_ = outer_coro;
                        // Resume the outer coro N-1 times to get all the inner coros started
                        bool do_suspend = 0 == --outer_coro.promise().started_;
                        coro_.resume();
                        return do_suspend;
                    }
                    when_all_inner &await_resume()
                    {
                        return *this;
                    }
                    co_result_not_void_t<T> get() const
                    {
                        return coro_.promise().get();
                    }
                };

                template <class... Ts, class T>
                when_all_inner<T, Ts...> do_when_all_inner(T &v)
                {
                    co_return co_await v;
                }

                template <class... Ts>
                std::tuple<co_result_not_void_t<Ts>...>
                do_when_all_collect(when_all_inner<Ts, Ts...> &... inners)
                {
                    return std::tuple<co_result_not_void_t<Ts>...>{inners.get()...};
                }

                struct when_all_fn
                {
                    // template <Awaitable ...Ts>
                    template <class... Ts>
                    when_all_awaitable<Ts...> operator()(Ts &&...us) const
                    {
                        co_return
                            do_when_all_collect<Ts...>(
                                co_await do_when_all_inner<Ts...>(us)...);
                    }
                };
            } // namespace detail
            /// \endcond

            RANGES_INLINE_VARIABLE(detail::when_all_fn, when_all)

        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_WHEN_ALL_HPP
