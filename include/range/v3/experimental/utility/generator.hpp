// Range v3 library
//
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_EXPERIMENTAL_UTILITY_GENERATOR_HPP
#define RANGES_V3_EXPERIMENTAL_UTILITY_GENERATOR_HPP

#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <cstddef>
#include <exception>
#include <utility>
#include <experimental/coroutine>
#include <meta/meta.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        namespace experimental
        {
            // The type of size() for a sized_generator
            using generator_size_t = std::size_t;

            // Type upon which to co_await to set the size of a sized_generator
            enum struct generator_size : generator_size_t { invalid = ~generator_size_t(0) };

            template<typename Promise = void>
            struct coroutine_owner;
        } // namespace experimental

        /// \cond
        namespace detail
        {
            inline void resume(std::experimental::coroutine_handle<> coro)
            {
                // Pre: coro refers to a suspended coroutine.
                RANGES_EXPECT(coro);
                RANGES_EXPECT(!coro.done());
                coro.resume();
            }

            namespace coroutine_owner_
            {
                struct adl_hook {};

                template<typename Promise>
                void swap(experimental::coroutine_owner<Promise> &x,
                          experimental::coroutine_owner<Promise> &y) noexcept
                {
                    ranges::swap(x.base(), y.base());
                }
            } // namespace coroutine_owner_
        } // namespace detail
        /// \endcond

        namespace experimental
        {
            // An owning coroutine_handle
            template<typename Promise>
            struct coroutine_owner
              : std::experimental::coroutine_handle<Promise>
              , private detail::coroutine_owner_::adl_hook
            {
                using base_t = std::experimental::coroutine_handle<Promise>;

                using base_t::operator bool;
                using base_t::done;
                using base_t::promise;

                constexpr coroutine_owner() noexcept = default;
                explicit constexpr coroutine_owner(base_t coro) noexcept
                  : base_t(coro)
                {}
                RANGES_CXX14_CONSTEXPR coroutine_owner(coroutine_owner &&that) noexcept
                  : base_t(ranges::exchange(that.base(), {}))
                {}

                ~coroutine_owner() { if (*this) base().destroy(); }

                coroutine_owner &operator=(coroutine_owner &&that) noexcept
                {
                    coroutine_owner{ranges::exchange(base(), ranges::exchange(that.base(), {}))};
                    return *this;
                }

                void resume() { detail::resume(base()); }
                void operator()() { detail::resume(base()); }

                base_t &base() noexcept { return *this; }
                base_t const &base() const noexcept { return *this; }

                base_t release() noexcept
                {
                    return ranges::exchange(base(), {});
                }
            };
        } // namespace experimental

        /// \cond
        namespace detail
        {
            template<typename Reference>
            struct generator_promise
            {
                std::exception_ptr except_ = nullptr;

                CONCEPT_ASSERT(meta::or_<std::is_reference<Reference>,
                    CopyConstructible<Reference>>());

                generator_promise *get_return_object() noexcept
                {
                    return this;
                }
                std::experimental::suspend_always initial_suspend() const noexcept
                {
                    return {};
                }
                std::experimental::suspend_always final_suspend() const noexcept
                {
                    return {};
                }
                void return_void() const noexcept
                {}
                void unhandled_exception() noexcept
                {
                    except_ = std::current_exception();
                    RANGES_EXPECT(except_);
                }
                template<typename Arg,
                    CONCEPT_REQUIRES_(ConvertibleTo<Arg, Reference>() &&
                        std::is_assignable<semiregular_t<Reference> &, Arg>::value)>
                std::experimental::suspend_always yield_value(Arg &&arg)
                    noexcept(std::is_nothrow_assignable<semiregular_t<Reference> &, Arg>::value)
                {
                    ref_ = std::forward<Arg>(arg);
                    return {};
                }
                std::experimental::suspend_never
                await_transform(experimental::generator_size) const noexcept
                {
                    RANGES_ENSURE_MSG(false, "Invalid size request for a non-sized generator");
                    return {};
                }
                meta::if_<std::is_reference<Reference>, Reference, Reference const &>
                read() const noexcept
                {
                    return ref_;
                }
            private:
                semiregular_t<Reference> ref_;
            };

            template<typename Reference>
            struct sized_generator_promise
              : generator_promise<Reference>
            {
                sized_generator_promise *get_return_object() noexcept
                {
                    return this;
                }
                std::experimental::suspend_never initial_suspend() const noexcept
                {
                    // sized_generator doesn't suspend at its initial suspend point because...
                    return {};
                }
                std::experimental::suspend_always
                await_transform(experimental::generator_size size) noexcept
                {
                    // ...we need the coroutine set the size of the range first by
                    // co_awaiting on a generator_size.
                    size_ = size;
                    return {};
                }
                experimental::generator_size_t size() const noexcept
                {
                    RANGES_EXPECT(size_ != experimental::generator_size::invalid);
                    return static_cast<experimental::generator_size_t>(size_);
                }
            private:
                experimental::generator_size size_ = experimental::generator_size::invalid;
            };
        } // namespace detail
        /// \endcond

        namespace experimental
        {
            template<typename Reference, typename Value = uncvref_t<Reference>>
            struct generator
              : view_facade<generator<Reference, Value>>
            {
                using promise_type = detail::generator_promise<Reference>;

                constexpr generator() noexcept = default;
                generator(promise_type *p)
                  : coro_{handle::from_promise(*p)}
                {
                    RANGES_EXPECT(coro_);
                }

            protected:
                coroutine_owner<promise_type> coro_;
            private:
                friend range_access;
                using handle = std::experimental::coroutine_handle<promise_type>;

                struct cursor
                {
                    using value_type = Value;

                    cursor() = default;
                    constexpr explicit cursor(handle coro) noexcept
                      : coro_{coro}
                    {}
                    bool equal(default_sentinel) const
                    {
                        RANGES_EXPECT(coro_);
                        if (coro_.done())
                        {
                            auto &e = coro_.promise().except_;
                            if (e) std::rethrow_exception(std::move(e));
                            return true;
                        }
                        return false;
                    }
                    void next()
                    {
                        detail::resume(coro_);
                    }
                    Reference read() const
                    {
                        RANGES_EXPECT(coro_);
                        return coro_.promise().read();
                    }
                private:
                    handle coro_ = nullptr;
                };

                cursor begin_cursor()
                {
                    detail::resume(coro_);
                    return cursor{coro_};
                }
            };

            template<typename Reference, typename Value = uncvref_t<Reference>>
            struct sized_generator
              : generator<Reference, Value>
            {
                using promise_type = detail::sized_generator_promise<Reference>;
                using handle = std::experimental::coroutine_handle<promise_type>;

                constexpr sized_generator() noexcept = default;
                sized_generator(promise_type *p)
                  : generator<Reference, Value>{p}
                {}
                generator_size_t size() const noexcept
                {
                    return promise().size();
                }
            private:
                using generator<Reference, Value>::coro_;

                promise_type const &promise() const noexcept
                {
                    RANGES_EXPECT(coro_);
                    return static_cast<promise_type const &>(coro_.promise());
                }
            };
        } // namespace experimental

        /// @}
    } // inline namespace v3
} // namespace ranges
#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1

#endif // RANGES_V3_EXPERIMENTAL_UTILITY_GENERATOR_HPP
