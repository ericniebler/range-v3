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

        /// \cond
        namespace adl_swap_detail
        {
            using std::swap;

            // Forward-declarations first!
            template<typename First0, typename Second0, typename First1, typename Second1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<is_swappable<First0, First1>::value &&
                       is_swappable<Second0, Second1>::value>
            swap(std::pair<First0, Second0> &&left, std::pair<First1, Second1> &&right)
                noexcept(is_nothrow_swappable<First0, First1>::value &&
                         is_nothrow_swappable<Second0, Second1>::value);

            template<typename ...Ts, typename ...Us>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<meta::and_c<is_swappable<Ts, Us>::value...>::value>
            swap(std::tuple<Ts...> &&left, std::tuple<Us...> &&right)
                noexcept(meta::and_c<is_nothrow_swappable<Ts, Us>::value...>::value);

            // Function wrapper here, now that declarations have been seen.
            struct swap_fn
            {
                template<typename T, typename U>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_swappable<T, U>::value>
                operator()(T && t, U && u) const noexcept(is_nothrow_swappable<T, U>::value)
                {
                    swap(std::forward<T>(t), std::forward<U>(u));
                }
            };

            // Now implementations
            template<typename T, typename U, typename Enable = void>
            struct is_swappable_
              : std::false_type
            {};

            template<typename T, typename U>
            struct is_swappable_<T, U, meta::void_<
                decltype(swap(std::declval<T>(), std::declval<U>())),
                decltype(swap(std::declval<U>(), std::declval<T>()))>>
              : std::true_type
            {};

            template<typename T, typename U>
            struct is_nothrow_swappable_
              : meta::bool_<noexcept(swap(std::declval<T>(), std::declval<U>()))>
            {};

            template<typename First0, typename Second0, typename First1, typename Second1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<is_swappable<First0, First1>::value &&
                       is_swappable<Second0, Second1>::value>
            swap(std::pair<First0, Second0> &&left, std::pair<First1, Second1> &&right)
                noexcept(is_nothrow_swappable<First0, First1>::value &&
                         is_nothrow_swappable<Second0, Second1>::value )
            {
                swap(std::move(left).first, std::move(right).first);
                swap(std::move(left).second, std::move(right).second);
            }

            template<typename ...Ts, typename ...Us, std::size_t ...Is>
            RANGES_CXX14_CONSTEXPR
            void tuple_swap_(std::tuple<Ts...> &&left, std::tuple<Us...> &&right, meta::index_sequence<Is...>)
            {
                detail::ignore_unused(
                    (swap(std::get<Is>(std::move(left)), std::get<Is>(std::move(right))), 42)...);
            }

            template<typename ...Ts, typename ...Us>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<meta::and_c<is_swappable<Ts, Us>::value...>::value>
            swap(std::tuple<Ts...> &&left, std::tuple<Us...> &&right)
                noexcept(meta::and_c<is_nothrow_swappable<Ts, Us>::value...>::value)
            {
                adl_swap_detail::tuple_swap_(std::move(left), std::move(right),
                    meta::make_index_sequence<sizeof...(Ts)>{});
            }

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
            template<typename Readable0, typename Readable1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                is_swappable<decltype(*std::declval<Readable0 &>()),
                             decltype(*std::declval<Readable1 &>())>::value>
            indirect_swap(Readable0 a, Readable1 b)
                noexcept(is_nothrow_swappable<decltype(*std::declval<Readable0 &>()),
                                              decltype(*std::declval<Readable1 &>())>::value);

            template<typename Readable0, typename Readable1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                !is_swappable<
                    decltype(*std::declval<Readable0 &>()),
                    decltype(*std::declval<Readable1 &>())>::value &&
                is_indirectly_movable<Readable0, Readable1>::value &&
                is_indirectly_movable<Readable1, Readable0>::value>
            indirect_swap(Readable0 a, Readable1 b)
                noexcept(
                    is_nothrow_indirectly_movable<Readable0, Readable1>::value &&
                    is_nothrow_indirectly_movable<Readable0, Readable1>::value);

            struct indirect_swap_fn
            {
                template<typename Readable0, typename Readable1>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_indirectly_swappable<Readable0, Readable1>::value>
                operator()(Readable0 a, Readable1 b) const
                    noexcept(is_nothrow_indirectly_swappable<Readable0, Readable1>::value)
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
            struct is_indirectly_swappable_<T, U, meta::if_c<
                std::is_void<
                    decltype(indirect_swap(std::declval<T>(), std::declval<U>()))>::value>>
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

            template<typename Readable0, typename Readable1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                is_swappable<decltype(*std::declval<Readable0 &>()),
                             decltype(*std::declval<Readable1 &>())>::value>
            indirect_swap(Readable0 a, Readable1 b)
                noexcept(is_nothrow_swappable<decltype(*std::declval<Readable0 &>()),
                                              decltype(*std::declval<Readable1 &>())>::value)
            {
                swap(*a, *b);
            }

            template<typename Readable0, typename Readable1>
            RANGES_CXX14_CONSTEXPR
            meta::if_c<
                !is_swappable<
                    decltype(*std::declval<Readable0 &>()),
                    decltype(*std::declval<Readable1 &>())>::value &&
                is_indirectly_movable<Readable0, Readable1>::value &&
                is_indirectly_movable<Readable1, Readable0>::value>
            indirect_swap(Readable0 a, Readable1 b)
                noexcept(
                    is_nothrow_indirectly_movable<Readable0, Readable1>::value &&
                    is_nothrow_indirectly_movable<Readable0, Readable1>::value)
            {
                meta::_t<value_type<Readable0>> v0 = indirect_move(a);
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
        namespace
        {
            constexpr auto&& swap = static_const<adl_swap_detail::swap_fn>::value;
        }

        /// \ingroup group-utility
        /// \relates adl_swap_detail::indirect_swap_fn
        namespace
        {
            constexpr auto&& indirect_swap = static_const<adl_swap_detail::indirect_swap_fn>::value;
        }
    }
}

#endif
