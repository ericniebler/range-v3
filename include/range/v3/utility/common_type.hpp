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

#ifndef RANGES_V3_UTILITY_COMMON_TYPE_HPP
#define RANGES_V3_UTILITY_COMMON_TYPE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>

// Sadly, this is necessary because of:
//  - std::common_type is not SFINAE-friendly, and
//  - The specification of std::common_type makes it impossibly
//    difficult to specialize on user-defined types without spamming
//    out a bajillion copies to handle all combinations of cv and ref
//    qualifiers.

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    template<typename...Ts>
    class tuple;
RANGES_END_NAMESPACE_STD
#else
#include <tuple>
#endif

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct promote_rvalue
              : meta::id<T>
            {};
            template<typename T>
            struct promote_rvalue<T &&>
              : meta::id<T const &>
            {};
            template<typename T>
            using promote_rvalue_t = meta::eval<promote_rvalue<T>>;

            // Work around GCC #51317
            // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51317
#if defined(__GNUC__) && !defined(__clang__)
            template<typename T, typename U>
            struct default_common_impl
            {
                template<typename T2, typename U2>
                using apply = decltype(true? std::declval<promote_rvalue_t<T2>>()
                                           : std::declval<promote_rvalue_t<U2>>());
            };
            template<typename T>
            struct default_common_impl<T, T>
            {
                template<typename, typename>
                using apply = T;
            };
            template<typename T>
            struct default_common_impl<T &&, T &&>
            {
                template<typename, typename>
                using apply = T &&;
            };
            template<typename T, typename U>
            struct default_common_impl<T &&, U &&>
            {
                template<typename T2, typename U2>
                using apply = decltype(true? std::declval<T2 &&>()
                                           : std::declval<U2 &&>());
            };
            template<typename T>
            struct default_common_impl<T &, T const &>
            {
                template<typename, typename>
                using apply = T const &;
            };
            template<typename T>
            struct default_common_impl<T const &, T &>
            {
                template<typename, typename>
                using apply = T const &;
            };
            template<typename T>
            struct default_common_impl<T &&, T const &&>
            {
                template<typename, typename>
                using apply = T const &&;
            };
            template<typename T>
            struct default_common_impl<T const &&, T &&>
            {
                template<typename, typename>
                using apply = T const &&;
            };
            template<typename T>
            struct default_common_impl<T &&, T const &>
            {
                template<typename, typename>
                using apply = T const &;
            };
            template<typename T>
            struct default_common_impl<T const &&, T &>
            {
                template<typename, typename>
                using apply = T const &;
            };
            template<typename T>
            struct default_common_impl<T &, T const &&>
            {
                template<typename, typename>
                using apply = T const &;
            };
            template<typename T>
            struct default_common_impl<T const &, T &&>
            {
                template<typename, typename>
                using apply = T const &;
            };

            template<typename T, typename U>
            using default_common_t =
                meta::eval<
                    meta::if_<
                        std::is_same<uncvref_t<T>, uncvref_t<U>>,
                        meta::lazy_apply<default_common_impl<T, U>, T, U>,
                        meta::id<decltype(true? std::declval<promote_rvalue_t<T>>()
                                              : std::declval<promote_rvalue_t<U>>())> > >;
#else
            template<typename T, typename U>
            struct default_common_impl
            {
                template<typename T2, typename U2>
                using apply = decltype(true? std::declval<promote_rvalue_t<T2>>()
                                           : std::declval<promote_rvalue_t<U2>>());
            };
            template<typename T>
            struct default_common_impl<T, T>
            {
                template<typename, typename>
                using apply = T;
            };
            template<typename T>
            struct default_common_impl<T &&, T &&>
            {
                template<typename, typename>
                using apply = T &&;
            };
            template<typename T, typename U>
            struct default_common_impl<T &&, U &&>
            {
                template<typename T2, typename U2>
                using apply = decltype(true? std::declval<T2 &&>()
                                           : std::declval<U2 &&>());
            };
            template<typename T, typename U>
            using default_common_t = meta::apply<default_common_impl<T, U>, T, U>;
#endif

            template<typename T, typename U, typename Enable = void>
            struct common_type_if
            {};

            template<typename T, typename U>
            struct common_type_if<T, U, void_t<default_common_t<T, U>>>
            {
                using type = decay_t<default_common_t<T, U>>;
            };

            template<typename T, typename U,
                     typename TT = decay_t<T>, typename UU = decay_t<U>>
            struct common_type2
              : common_type<TT, UU> // Recurse to catch user specializations
            {};

            template<typename T, typename U>
            struct common_type2<T, U, T, U>
              : common_type_if<T, U>
            {};

            template<typename Meta, typename Enable = void>
            struct has_type
              : std::false_type
            {};

            template<typename Meta>
            struct has_type<Meta, void_t<typename Meta::type>>
              : std::true_type
            {};

            template<typename Meta, typename...Ts>
            struct common_type_recurse
              : common_type<typename Meta::type, Ts...>
            {};

            template<typename Meta, typename...Ts>
            struct common_type_recurse_if
              : meta::if_<
                    has_type<Meta>,
                    common_type_recurse<Meta, Ts...>,
                    empty>
            {};
        }
        /// \endcond

        /// \addtogroup group-utility Utility
        /// @{
        ///

        /// Users should specialize this to hook the \c Common concept
        /// until \c std gets a SFINAE-friendly \c std::common_type and there's
        /// some sane way to deal with cv and ref qualifiers.
        template<typename ...Ts>
        struct common_type
        {};

        template<typename T>
        struct common_type<T>
        {
            using type = detail::decay_t<T>;
        };

        template<typename T, typename U>
        struct common_type<T, U>
          : detail::common_type2<T, U>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_type<T, U, Vs...>
          : detail::common_type_recurse_if<common_type<T, U>, Vs...>
        {};
        /// @}

        /// \addtogroup group-utility Utility
        /// @{
        ///

        namespace qual
        {
            using value_t =
                meta::quote_trait<meta::id>;

            using rvalue_ref_t =
                meta::quote_trait<std::add_rvalue_reference>;

            using const_rvalue_ref_t =
                meta::compose<
                    meta::quote_trait<std::add_rvalue_reference>,
                    meta::quote_trait<std::add_const>>;

            using lvalue_ref_t =
                meta::quote_trait<std::add_lvalue_reference>;

            using const_lvalue_ref_t =
                meta::compose<
                    meta::quote_trait<std::add_lvalue_reference>,
                    meta::quote_trait<std::add_const>>;
        }

        /// Users should specialize this to hook the \c CommonReference concept.
        template<typename T, typename U, typename TQual, typename UQual>
        struct common_reference_base
        {};

        /// \cond
        namespace detail
        {
            template<typename T>
            struct transform_reference
            {
                using type = qual::value_t;
            };

            template<typename T>
            struct transform_reference<T &&>
            {
                using type = qual::rvalue_ref_t;
            };

            template<typename T>
            struct transform_reference<T const &&>
            {
                using type = qual::const_rvalue_ref_t;
            };

            template<typename T>
            struct transform_reference<T &>
            {
                using type = qual::lvalue_ref_t;
            };

            template<typename T>
            struct transform_reference<T const &>
            {
                using type = qual::const_lvalue_ref_t;
            };

            template<typename T, typename U>
            using common_reference_base_ =
                common_reference_base<
                    uncvref_t<T>,
                    uncvref_t<U>,
                    meta::eval<transform_reference<T>>,
                    meta::eval<transform_reference<U>>>;

            template<typename T, typename U, typename Enable = void>
            struct common_reference_if
              : common_reference_base_<T, U>
            {};

            template<typename T, typename U>
            struct common_reference_if<T, U, void_t<default_common_t<T, U>>>
              : meta::if_<
                    meta::or_<
                        std::is_reference<default_common_t<T, U> >,
                        meta::not_<has_type<common_reference_base_<T, U> > > >,
                    meta::id<default_common_t<T, U> >,
                    common_reference_base_<T, U> >
            {};

            template<typename Meta, typename...Ts>
            struct common_reference_recurse
              : common_reference<typename Meta::type, Ts...>
            {};

            template<typename Meta, typename...Ts>
            struct common_reference_recurse_if
              : meta::if_<
                    has_type<Meta>,
                    common_reference_recurse<Meta, Ts...>,
                    empty>
            {};
        }
        /// \endcond

        /// Users should specialize this to hook the \c CommonReference concept.
        template<typename ...Ts>
        struct common_reference
        {};

        template<typename T>
        struct common_reference<T>
        {
            using type = T;
        };

        template<typename T, typename U>
        struct common_reference<T, U>
          : detail::common_reference_if<T, U>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_reference<T, U, Vs...>
          : detail::common_reference_recurse_if<common_reference<T, U>, Vs...>
        {};
        /// @}

        /// \cond
        // Specializations for pair and tuple
        template<typename F, typename S>
        struct common_pair;

        template<typename ...Ts>
        struct common_tuple;

        namespace detail
        {
            template<typename T, typename U, typename TupleLike>
            struct common_tuple_like;

            template<template<typename...> class T0, typename ...Ts,
                     template<typename...> class T1, typename ...Us,
                     typename TupleLike>
            struct common_tuple_like<T0<Ts...>, T1<Us...>, TupleLike>
              : meta::if_<
                    meta::and_<detail::has_type<common_type<Ts, Us> >...>,
                    meta::lazy_apply<
                        meta::compose<
                            meta::uncurry<TupleLike>,
                            meta::bind_back<meta::quote<meta::transform>, meta::quote<meta::eval> > >,
                        meta::list<common_type<Ts, Us>...> >,
                    detail::empty>
            {};
        }

        // common_type for std::pairs
        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<std::pair<F1, S1>, std::pair<F2, S2>>
          : detail::common_tuple_like<std::pair<F1, S1>, std::pair<F2, S2>, meta::quote<std::pair>>
        {};

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<std::pair<F1, S1>, common_pair<F2, S2>>
          : detail::common_tuple_like<std::pair<F1, S1>, common_pair<F2, S2>, meta::quote<common_pair>>
        {};

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<common_pair<F1, S1>, std::pair<F2, S2>>
          : detail::common_tuple_like<common_pair<F1, S1>, std::pair<F2, S2>, meta::quote<common_pair>>
        {};

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<common_pair<F1, S1>, common_pair<F2, S2>>
          : detail::common_tuple_like<common_pair<F1, S1>, common_pair<F2, S2>, meta::quote<common_pair>>
        {};

        // common_type for std::tuples
        template<typename ...Ts, typename ...Us>
        struct common_type<std::tuple<Ts...>, std::tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<std::tuple<Ts...>, std::tuple<Us...>, meta::quote<std::tuple> >,
                detail::empty>
        {};

        template<typename ...Ts, typename ...Us>
        struct common_type<common_tuple<Ts...>, std::tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<common_tuple<Ts...>, std::tuple<Us...>, meta::quote<common_tuple> >,
                detail::empty>
        {};

        template<typename ...Ts, typename ...Us>
        struct common_type<std::tuple<Ts...>, common_tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<std::tuple<Ts...>, common_tuple<Us...>, meta::quote<common_tuple> >,
                detail::empty>
        {};

        template<typename ...Ts, typename ...Us>
        struct common_type<common_tuple<Ts...>, common_tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<common_tuple<Ts...>, common_tuple<Us...>, meta::quote<common_tuple> >,
                detail::empty>
        {};

        namespace detail
        {
            template<typename T, typename U, typename TupleLike>
            struct common_tuple_like_ref;

            template<template<typename...> class T0, typename ...Ts,
                     template<typename...> class T1, typename ...Us, typename TupleLike>
            struct common_tuple_like_ref<T0<Ts...>, T1<Us...>, TupleLike>
              : meta::if_<
                    meta::and_<detail::has_type<common_reference<Ts, Us> >...>,
                    meta::lazy_apply<
                        meta::compose<
                            meta::uncurry<TupleLike>,
                            meta::bind_back<meta::quote<meta::transform>, meta::quote<meta::eval> > >,
                        meta::list<common_reference<Ts, Us>...> >,
                    detail::empty>
            {};
        }

        // A common reference for std::pairs
        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<std::pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                std::pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                std::pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<common_pair> >
        {};

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<common_pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                common_pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                std::pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<common_pair> >
        {};

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<std::pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                std::pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                common_pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<common_pair> >
        {};

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<common_pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                common_pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                common_pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<common_pair> >
        {};

        // A common reference for std::tuples
        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<std::tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    std::tuple<meta::apply<Qual1, Ts>...>,
                    std::tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<common_tuple> >,
                detail::empty>
        {};

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<common_tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    common_tuple<meta::apply<Qual1, Ts>...>,
                    std::tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<common_tuple> >,
                detail::empty>
        {};

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<std::tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    std::tuple<meta::apply<Qual1, Ts>...>,
                    common_tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<common_tuple> >,
                detail::empty>
        {};

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<common_tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    common_tuple<meta::apply<Qual1, Ts>...>,
                    common_tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<common_tuple> >,
                detail::empty>
        {};
        /// \endcond

    }
}

#endif
