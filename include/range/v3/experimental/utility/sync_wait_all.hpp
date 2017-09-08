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
                    bool is_set_{false};
                    alignas(value_type) char buffer_[sizeof(value_type)];
                    template<typename U>
                    void return_value(U &&u)
                    {
                        RANGES_EXPECT(!is_set_);
                        ::new((void*)&buffer_) value_type(static_cast<U &&>(u));
                        is_set_ = true;
                    }
                    ~sync_wait_all_value_promise()
                    {
                        if(is_set_)
                            static_cast<value_type *>((void*)&buffer_)->~value_type();
                    }
                    T &&get()
                    {
                        this->sync_wait_all_promise_base::get();
                        return static_cast<T &&>(*static_cast<value_type *>((void*)&buffer_));
                    }
                };

                template<typename T>
                using co_result_not_void_t =
                    meta::if_<std::is_void<co_result_t<T>>, void_, co_result_t<T>>;

                template<typename... Ts>
                struct wait_all_outer
                {
                    // In wait_all_inner's final_suspend, signal a condition variable. Then in
                    // this awaitable's wait() function, wait on the condition variable.
                    struct promise_type
                      : detail::sync_wait_all_value_promise<std::tuple<co_result_not_void_t<Ts>...>>
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
                        wait_all_outer get_return_object() noexcept
                        {
                            return wait_all_outer{*this};
                        }
                    };
                    using coro_handle_t = std::experimental::coroutine_handle<promise_type>;
                    coro_handle_t coro_;

                    explicit wait_all_outer(promise_type &p) noexcept
                      : coro_(coro_handle_t::from_promise(p))
                    {}
                    wait_all_outer(wait_all_outer &&that) noexcept
                      : coro_(ranges::exchange(that.coro_, nullptr))
                    {}
                    ~wait_all_outer()
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
                struct wait_all_inner
                {
                    using outer_promise_type_t = typename wait_all_outer<Ts...>::promise_type;
                    using outer_coro_handle_t = std::experimental::coroutine_handle<outer_promise_type_t>;

                    struct promise_type
                      : meta::if_<
                            std::is_void<co_result_t<T>>,
                            sync_wait_all_void_promise,
                            sync_wait_all_value_promise<co_result_t<T>>>
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
                        wait_all_inner get_return_object() noexcept
                        {
                            return wait_all_inner{*this};
                        }
                    };

                    using coro_handle = std::experimental::coroutine_handle<promise_type>;
                    coro_handle coro_;

                    explicit wait_all_inner(promise_type &p) noexcept
                      : coro_(coro_handle::from_promise(p))
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
                    bool await_suspend(outer_coro_handle_t outer_coro) noexcept
                    {
                        coro_.promise().outer_coro_ = outer_coro;
                        coro_.resume();
                        // Resume the outer coro N-1 times to get all the inner coros started
                        return 0 == --outer_coro.promise().started_;
                    }
                    wait_all_inner await_resume()
                    {
                        return std::move(*this);
                    }
                    co_result_not_void_t<T> get() const
                    {
                        return coro_.promise().get();
                    }
                };
            } // namespace detail
            /// \endcond

            // template <Awaitable ...Ts>
            template<typename ...Ts,
                CONCEPT_REQUIRES_(meta::and_<Awaitable<Ts>...>())>
            std::tuple<detail::co_result_not_void_t<Ts>...> sync_wait_all(Ts &&...ts)
            {
                return sync_wait([](Ts &...us) -> detail::wait_all_outer<Ts...> {
                    co_return [](detail::wait_all_inner<Ts, Ts...>... inners) {
                        return std::tuple<detail::co_result_not_void_t<Ts>...>{
                            std::move(inners).get()...
                        };
                    }(co_await [](Ts &v) -> detail::wait_all_inner<Ts, Ts...> {
                        co_return co_await v;
                    }(us)...);
                }(ts...));
            }
        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_SYNC_WAIT_ALL_HPP
