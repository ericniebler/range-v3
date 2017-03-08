/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// The implementation of swap (see below) has been adapted from libc++
// (http://libcxx.llvm.org).

#ifndef RANGES_V3_UTILITY_SWAP_HPP
#define RANGES_V3_UTILITY_SWAP_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/associated_types.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T, typename U = T>
        struct is_swappable;

        template<typename T, typename U = T>
        struct is_nothrow_swappable;

        template<typename T, typename U = T>
        struct is_indirectly_swappable;

        template<typename T, typename U = T>
        struct is_nothrow_indirectly_swappable;

        template <class T, class U = T>
        RANGES_CXX14_CONSTEXPR
        meta::if_c<
            std::is_constructible<T, T>::value &&
            std::is_assignable<T &, U>::value, T>
        exchange(T &t, U &&u)
            noexcept(
                std::is_nothrow_constructible<T, T>::value &&
                std::is_nothrow_assignable<T &, U>::value)
        {
            T tmp((T &&) t);
            t = (U &&) u;
            return tmp;
        }

        /// \cond
        namespace adl_swap_detail
        {
            // Intentionally create an ambiguity with std::swap, which is
            // (possibly) unconstrained.
            template<typename T>
            void swap(T &, T &) = delete;

            template<typename T, std::size_t N>
            void swap(T (&)[N], T (&)[N]) = delete;

            template<typename T, typename U,
                typename = decltype(swap(std::declval<T &&>(), std::declval<U &&>()))>
            std::true_type try_swap_(int);

            template<typename T, typename U>
            std::false_type try_swap_(long);

            template<typename T, typename U>
            struct is_swap_customized_
              : meta::id_t<decltype(adl_swap_detail::try_swap_<T, U>(42))>
            {};

            struct swap_fn
            {
                // Dispatch to customization point:
                template <class T, class U>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_swap_customized_<T, U>::value>
                operator()(T &&t, U &&u) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    (void)swap((T &&) t, (U &&) u)
                )

                // Handle swappable types
                template <class T>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<
                    !is_swap_customized_<T &, T &>::value &&
                    std::is_constructible<T, T>::value &&
                    std::is_assignable<T &, T>::value>
                operator()(T &a, T &b) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    (void)(b = ranges::exchange(a, (T &&) b))
                )

                // Handle arrays
                template <class T, class U, std::size_t N>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<
                    !is_swap_customized_<T (&)[N], U (&)[N]>::value &&
                    is_swappable<T &, U &>::value>
                operator()(T (&t)[N], U (&u)[N]) const
                    noexcept(is_nothrow_swappable<T, U>::value)
                {
                    for(std::size_t i = 0; i < N; ++i)
                        (*this)(t[i], u[i]);
                }

                template<typename F0, typename S0, typename F1, typename S1>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_swappable<F0, F1>::value && is_swappable<S0, S1>::value>
                operator()(std::pair<F0, S0> &&left, std::pair<F1, S1> &&right) const
                    noexcept(
                        is_nothrow_swappable<F0, F1>::value &&
                        is_nothrow_swappable<S0, S1>::value)
                {
                    swap_fn{}(std::move(left).first, std::move(right).first);
                    swap_fn{}(std::move(left).second, std::move(right).second);
                }

                template<typename ...Ts, typename ...Us>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<meta::and_c<is_swappable<Ts, Us>::value...>::value>
                operator()(std::tuple<Ts...> &&left, std::tuple<Us...> &&right) const
                    noexcept(meta::and_c<is_nothrow_swappable<Ts, Us>::value...>::value)
                {
                    swap_fn::impl(detail::move(left), detail::move(right),
                        meta::make_index_sequence<sizeof...(Ts)>{});
                }

            private:
                template<typename T, typename U, std::size_t ...Is>
                RANGES_CXX14_CONSTEXPR
                static void impl(T &&left, U &&right, meta::index_sequence<Is...>)
                {
                    (void) detail::ignore_unused(
                        (swap_fn{}(std::get<Is>(detail::move(left)),
                                   std::get<Is>(detail::move(right))), 42)...);
                }
            };

            // Now implementations
            template<typename T, typename U, typename = void>
            struct is_swappable_
              : std::false_type
            {};

            template<typename T, typename U>
            struct is_swappable_<T, U, meta::void_<
                decltype(swap_fn{}(std::declval<T>(), std::declval<U>())),
                decltype(swap_fn{}(std::declval<U>(), std::declval<T>()))>>
              : std::true_type
            {};

            template<typename T, typename U>
            struct is_nothrow_swappable_
              : meta::bool_<noexcept(swap_fn{}(std::declval<T>(), std::declval<U>()))>
            {};

            // Q: Should std::reference_wrapper be considered a proxy wrt swapping rvalues?
            // A: No. Its operator= is currently defined to reseat the references, so
            //    std::swap(ra, rb) already means something when ra and rb are (lvalue)
            //    reference_wrappers. That reseats the reference wrappers but leaves the
            //    referents unmodified. Treating rvalue reference_wrappers differently would
            //    be confusing.

            // Q: Then why is it OK to "re"-define swap for pairs and tuples of references?
            // A: Because as defined above, swapping an rvalue tuple of references has the same
            //    semantics as swapping an lvalue tuple of references. Rather than reseat the
            //    references, assignment happens *through* the references.

            // Q: But I have an iterator whose operator* returns an rvalue
            //    std::reference_wrapper<T>. How do I make it model IndirectlySwappable?
            // A: With an overload of indirect_swap.

            // Forward-declarations first!
            template<typename I0, typename I1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                is_swappable<decltype(*std::declval<I0 &>()),
                             decltype(*std::declval<I1 &>())>::value>
            indirect_swap(I0 a, I1 b)
                noexcept(is_nothrow_swappable<decltype(*std::declval<I0 &>()),
                                              decltype(*std::declval<I1 &>())>::value);

            template<typename I0, typename I1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                !is_swappable<
                    decltype(*std::declval<I0 &>()),
                    decltype(*std::declval<I1 &>())>::value &&
                is_indirectly_movable<I0, I1>::value &&
                is_indirectly_movable<I1, I0>::value>
            indirect_swap(I0 a, I1 b)
                noexcept(
                    is_nothrow_indirectly_movable<I0, I1>::value &&
                    is_nothrow_indirectly_movable<I1, I0>::value);

            struct indirect_swap_fn
            {
                template<typename I0, typename I1>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_indirectly_swappable<I0, I1>::value>
                operator()(I0 a, I1 b) const
                    noexcept(is_nothrow_indirectly_swappable<I0, I1>::value)
                {
                    indirect_swap(std::move(a), std::move(b));
                }
            };

            // Now implementations
            template<typename T, typename U, typename Enable = void>
            struct is_indirectly_swappable_
              : std::false_type
            {};

            template<typename T, typename U>
            struct is_indirectly_swappable_<T, U, meta::void_<
                decltype(indirect_swap(std::declval<T>(), std::declval<U>()))>>
              : std::true_type
            {};

            template<typename T, typename U>
            struct is_nothrow_indirectly_swappable_
              : meta::bool_<noexcept(indirect_swap(std::declval<T>(), std::declval<U>()))>
            {};

            // Q: Why isn't this called "iter_swap"?
            // A: Because it needs to be findable with ADL, and if we call it iter_swap and
            //    ::std is an associated namespace of one of the arguments, then it'll find
            //    the unconstrained std::iter_swap, which we don't want. The real fix is to
            //    properly constrain std::iter_swap and rename this.

            template<typename I0, typename I1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                is_swappable<decltype(*std::declval<I0 &>()),
                             decltype(*std::declval<I1 &>())>::value>
            indirect_swap(I0 a, I1 b)
                noexcept(is_nothrow_swappable<decltype(*std::declval<I0 &>()),
                                              decltype(*std::declval<I1 &>())>::value)
            {
                swap_fn{}(*a, *b);
            }

            template<typename I0, typename I1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                !is_swappable<
                    decltype(*std::declval<I0 &>()),
                    decltype(*std::declval<I1 &>())>::value &&
                is_indirectly_movable<I0, I1>::value &&
                is_indirectly_movable<I1, I0>::value>
            indirect_swap(I0 a, I1 b)
                noexcept(
                    is_nothrow_indirectly_movable<I0, I1>::value &&
                    is_nothrow_indirectly_movable<I1, I0>::value)
            {
                meta::_t<value_type<I0>> v0 = indirect_move(a);
                *a = indirect_move(b);
                *b = std::move(v0);
            }
        }
        /// \endcond

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_swappable
          : adl_swap_detail::is_swappable_<T, U>
        {};

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_nothrow_swappable
          : meta::and_<
                is_swappable<T, U>,
                adl_swap_detail::is_nothrow_swappable_<T, U>>
        {};

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_indirectly_swappable
          : adl_swap_detail::is_indirectly_swappable_<T, U>
        {};

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_nothrow_indirectly_swappable
          : meta::and_<
                is_indirectly_swappable<T, U>,
                adl_swap_detail::is_nothrow_indirectly_swappable_<T, U>>
        {};

        /// \ingroup group-utility
        /// \relates adl_swap_detail::swap_fn
        RANGES_INLINE_VARIABLE(adl_swap_detail::swap_fn, swap)

        /// \ingroup group-utility
        /// \relates adl_swap_detail::indirect_swap_fn
        RANGES_INLINE_VARIABLE(adl_swap_detail::indirect_swap_fn, indirect_swap)
    }
}

#endif
