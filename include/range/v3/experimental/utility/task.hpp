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
// Inspired by
// https://channel9.msdn.com/events/CPP/CppCon-2016/CppCon-2016-Gor-Nishanov-C-Coroutines-Under-the-covers
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_TASK_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_TASK_HPP

#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <new>
#include <exception>
#include <type_traits>
#include <experimental/coroutine>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace experimental
        {
            /// \cond
            namespace detail
            {
                [[noreturn]] inline void unreachable()
                {
                    RANGES_ASSERT(false && "Reached unreachable code!");
                    std::terminate();
                }

                template<typename T = void>
                struct promise_base
                {
                    enum struct which
                    {
                        none,
                        value,
                        error
                    };
                    using value_type = meta::if_<
                        std::is_reference<T>,
                        reference_wrapper<meta::_t<std::remove_reference<T>>, std::is_rvalue_reference<T>::value>,
                        T>;
                    which which_{which::none};
                    meta::_t<std::aligned_union<0, value_type, std::exception_ptr>> buffer_;

                    template<typename U,
                        CONCEPT_REQUIRES_(Constructible<value_type, U>())>
                    void return_value(U &&value)
                    {
                        RANGES_EXPECT(which_ == which::none);
                        ::new((void *)&buffer_) value_type(static_cast<U &&>(value));
                        which_ = which::value;
                    }
                    void unhandled_exception()
                    {
                        RANGES_EXPECT(which_ == which::none);
                        ::new((void *)&buffer_) std::exception_ptr(std::current_exception());
                        which_ = which::error;
                    }
                    T await_resume()
                    {
                        switch(which_)
                        {
                        case which::value:
                            return *static_cast<value_type *>((void *)&buffer_);
                        case which::error:
                            std::rethrow_exception(
                                *static_cast<std::exception_ptr *>((void *)&buffer_));
                        case which::none:
                            unreachable();
                        }
                    }
                    ~promise_base()
                    {
                        if(which_ == which::value)
                            static_cast<value_type *>((void *)&buffer_)->~value_type();
                        else if(which_ == which::error)
                            static_cast<std::exception_ptr *>((void *)&buffer_)->~exception_ptr();
                    }
                };

                template<>
                struct promise_base<void>
                {
                    std::exception_ptr eptr;
                    void return_void()
                    {}
                    void unhandled_exception()
                    {
                        eptr = std::current_exception();
                    }
                    void await_resume()
                    {
                        if(eptr)
                            std::rethrow_exception(eptr);
                    }
                };
            } // namespace detail
            /// \endcond

            template<typename T /* = void */>
            struct task
            {
                static_assert(!std::is_function<T>::value, "can't create a task that returns a function type");
                struct promise_type;
            private:
                struct final_suspend_result
                {
                    promise_type *me_;
                    bool await_ready()
                    {
                        return false;
                    }
                    void await_suspend(std::experimental::coroutine_handle<>)
                    {
                        me_->waiter.resume();
                    }
                    void await_resume()
                    {}
                };

                std::experimental::coroutine_handle<promise_type> coro_;

                explicit task(promise_type &p) noexcept
                  : coro_(std::experimental::coroutine_handle<promise_type>::from_promise(p))
                {}
            public:
                struct promise_type
                  : detail::promise_base<T>
                {
                    std::experimental::coroutine_handle<> waiter;

                    task get_return_object()
                    {
                        return task{*this};
                    }
                    std::experimental::suspend_always initial_suspend()
                    {
                        return {};
                    }
                    final_suspend_result final_suspend()
                    {
                        return final_suspend_result{this};
                    }
                };

                task(task &&that) noexcept
                  : coro_(ranges::exchange(that.coro_, nullptr))
                {}
                ~task()
                {
                    if(coro_)
                        coro_.destroy();
                }
                bool await_ready()
                {
                    return coro_.done();
                }
                void await_suspend(std::experimental::coroutine_handle<> waiter)
                {
                    coro_.promise().waiter = waiter;
                    coro_.resume();
                }
                T await_resume()
                {
                    return coro_.promise().await_resume();
                }
            };

            template<typename T>
            struct task_iterator
            {
            private:
                task<T> *task_ = nullptr;
            public:
                task_iterator() = default;
                explicit task_iterator(task<T> &t) noexcept
                  : task_(&t)
                {}
                task_iterator(task_iterator &&that) noexcept
                  : task_(ranges::exchange(that.task_, nullptr))
                {}
                task_iterator &operator=(task_iterator &&that) noexcept
                {
                    task_ = ranges::exchange(that.task_, nullptr);
                    return &this;
                }
                T operator*() const
                {
                    return task_->await_resume();
                }
                task<task_iterator &> operator++()
                {
                    task_ = nullptr;
                    co_return *this;
                }
                task<> operator++(int)
                {
                    task_ = nullptr;
                    co_return;
                }
                friend bool operator==(task_iterator const &x, task_iterator const &y)
                {
                    return x.task_ == y.task_;
                }
                friend bool operator!=(task_iterator const &x, task_iterator const &y)
                {
                    return !(x == y);
                }
            };

            /// \cond
            namespace detail
            {
                template<typename T>
                struct task_begin
                {
                    task<T> &task_;

                    bool await_ready()
                    {
                        return task_.await_ready();
                    }
                    void await_suspend(std::experimental::coroutine_handle<> waiter)
                    {
                        task_.await_suspend(waiter);
                    }
                    task_iterator<T> await_resume()
                    {
                        return task_iterator<T>(task_);
                    }
                };
            }
            /// \endcond

            template<typename T,
                CONCEPT_REQUIRES_(!std::is_void<T>::value)>
            detail::task_begin<T> begin(task<T> &t) noexcept
            {
                return detail::task_begin<T>{t};
            }

            template<typename T,
                CONCEPT_REQUIRES_(!std::is_void<T>::value)>
            task_iterator<T> end(task<T> &) noexcept
            {
                return task_iterator<T>{};
            }
        }
    }
}

#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#endif // RANGES_V3_EXPERIMENTAL_UTILITY_TASK_HPP
