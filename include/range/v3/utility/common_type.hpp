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
            template<typename From, typename To>
            struct copy_cv
            {
                using type = To;
            };
            template<typename From, typename To>
            struct copy_cv<From const, To>
            {
                using type = To const;
            };
            template<typename From, typename To>
            struct copy_cv<From volatile, To>
            {
                using type = To volatile;
            };
            template<typename From, typename To>
            struct copy_cv<From const volatile, To>
            {
                using type = To const volatile;
            };
            template<typename From, typename To>
            using copy_cv_t = meta::eval<copy_cv<From, To>>;

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename U>
            struct builtin_common_impl;

        #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
            template<typename T, typename U>
            using conditional_result_t = decltype(true ? std::declval<T>() : std::declval<U>());

            template<typename T, typename U>
            using builtin_common_t = meta::apply<builtin_common_impl<T, U>>;

            template<typename T, typename U>
            struct builtin_common_impl
            {
                template<typename X = T, typename Y = U>
                using apply = decay_t<conditional_result_t<as_cref_t<X>, as_cref_t<Y>>>;
            };
            template<typename T, typename U>
            struct builtin_common_impl<T &&, U &&>
            {
                template<typename X = T, typename Y = U, typename R = builtin_common_t<X &, Y &>>
                using apply =
                    meta::if_<std::is_reference<R>, meta::eval<std::remove_reference<R>> &&, R>;
            };
            template<typename T, typename U>
            struct builtin_common_impl<T &, U &>
            {
                template<typename X = T, typename Y = U>
                using apply = conditional_result_t<copy_cv_t<Y, X> &, copy_cv_t<X, Y> &>;
            };
            template<typename T, typename U>
            struct builtin_common_impl<T &, U &&>
              : builtin_common_impl<T &, U const &>
            {};
            template<typename T, typename U>
            struct builtin_common_impl<T &&, U &>
              : builtin_common_impl<T const &, U &>
            {};
        #else
            template<typename A, typename B, typename = void>
            struct conditional_result
            {};
            template<typename A, typename B>
            struct conditional_result<A, B, meta::void_<decltype(true ? std::declval<A>() : std::declval<B>())>>
            {
                using type = decltype(true ? std::declval<A>() : std::declval<B>());
            };

            template<typename T, typename U>
            using builtin_common_t = meta::eval<meta::apply<builtin_common_impl<T, U>>>;

            template<typename T, typename U>
            struct builtin_common_impl
            {
                template<typename = T, typename = U, typename = void>
                struct apply
                {};
                template<typename X, typename Y>
                struct apply<X, Y, meta::void_<meta::eval<conditional_result<as_cref_t<X>, as_cref_t<Y>>>>>
                {
                    using type = decay_t<meta::eval<conditional_result<as_cref_t<X>, as_cref_t<Y>>>>;
                };
            };
            template<typename T, typename U>
            struct builtin_common_impl<T &&, U &&>
            {
                template<typename = T, typename = U, typename = void>
                struct apply
                {};
                template<typename X, typename Y>
                struct apply<X, Y, meta::void_<builtin_common_t<X &, Y &>>>
                {
                    using R = builtin_common_t<X &, Y &>;
                    using type =
                        meta::if_<std::is_reference<R>, meta::eval<std::remove_reference<R>> &&, R>;
                };
            };
            template<typename T, typename U>
            struct builtin_common_impl<T &, U &>
            {
                template<typename X = T, typename Y = U>
                using apply = conditional_result<copy_cv_t<Y, X> &, copy_cv_t<X, Y> &>;
            };
            template<typename T, typename U>
            struct builtin_common_impl<T &, U &&>
              : builtin_common_impl<T &, U const &>
            {};
            template<typename T, typename U>
            struct builtin_common_impl<T &&, U &>
              : builtin_common_impl<T const &, U &>
            {};

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename U, typename Enable = void>
            struct common_type_if
            {};

            template<typename T, typename U>
            struct common_type_if<T, U, meta::void_<builtin_common_t<T, U>>>
            {
                using type = decay_t<builtin_common_t<T, U>>;
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

            template<typename Meta, typename...Ts>
            struct common_type_recurse
              : common_type<typename Meta::type, Ts...>
            {};

            template<typename Meta, typename...Ts>
            struct common_type_recurse_if
              : meta::if_<
                    meta::has_type<Meta>,
                    common_type_recurse<Meta, Ts...>,
                    meta::nil_>
            {};
        #endif

            template<typename T, typename U>
            using lazy_builtin_common_t = meta::defer<builtin_common_t, T, U>;
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
          : std::decay<T>
        {};

    #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
        template<typename T, typename U>
        struct common_type<T, U>
          : meta::if_c<
                ( std::is_same<detail::decay_t<T>, T>::value &&
                  std::is_same<detail::decay_t<U>, U>::value ),
                meta::lazy::let<detail::lazy_builtin_common_t<T, U>>,
                common_type<detail::decay_t<T>, detail::decay_t<U>>>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_type<T, U, Vs...>
          : meta::lazy::let<meta::lazy::fold<meta::list<U, Vs...>, T, meta::quote<common_type_t>>>
        {};
    #else
        template<typename T, typename U>
        struct common_type<T, U>
          : detail::common_type2<T, U>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_type<T, U, Vs...>
          : detail::common_type_recurse_if<common_type<T, U>, Vs...>
        {};
    #endif
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

        /// Users can specialize this to hook the \c CommonReference concept.
        /// \sa `common_reference`
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

        #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
        #else
            template<typename T, typename U, typename Enable = void>
            struct common_reference_if
              : common_reference_base_<T, U>
            {};

            template<typename T, typename U>
            struct common_reference_if<T, U, meta::void_<builtin_common_t<T, U>>>
              : meta::if_<
                    meta::or_<
                        std::is_reference<builtin_common_t<T, U> >,
                        meta::not_<meta::has_type<common_reference_base_<T, U> > > >,
                    meta::id<builtin_common_t<T, U> >,
                    common_reference_base_<T, U> >
            {};

            template<typename Meta, typename...Ts>
            struct common_reference_recurse
              : common_reference<typename Meta::type, Ts...>
            {};

            template<typename Meta, typename...Ts>
            struct common_reference_recurse_if
              : meta::if_<
                    meta::has_type<Meta>,
                    common_reference_recurse<Meta, Ts...>,
                    meta::nil_>
            {};
        #endif
        }
        /// \endcond

        /// Users can specialize this to hook the \c CommonReference concept.
        /// \sa `common_reference_base`
        template<typename ...Ts>
        struct common_reference
        {};

        template<typename T>
        struct common_reference<T>
        {
            using type = T;
        };

    #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
        template<typename T, typename U>
        struct common_reference<T, U>
          : meta::if_<
                meta::and_<
                    meta::has_type<detail::lazy_builtin_common_t<T, U>>,
                    meta::lazy::let<meta::lazy::or_<
                        std::is_reference<detail::lazy_builtin_common_t<T, U>>,
                        meta::not_<meta::has_type<detail::common_reference_base_<T, U>>>>>>,
                detail::lazy_builtin_common_t<T, U>,
                detail::common_reference_base_<T, U>>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_reference<T, U, Vs...>
          : meta::lazy::let<meta::lazy::fold<meta::list<U, Vs...>, T,
                meta::quote<common_reference_t>>>
        {};
    #else
        template<typename T, typename U>
        struct common_reference<T, U>
          : detail::common_reference_if<T, U>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_reference<T, U, Vs...>
          : detail::common_reference_recurse_if<common_reference<T, U>, Vs...>
        {};
    #endif
        /// @}

        /// \cond
        // Specializations for pair and tuple
        template<typename F, typename S>
        struct common_pair;

        template<typename ...Ts>
        struct common_tuple;

        // common_type for std::pairs
        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<std::pair<F1, S1>, common_pair<F2, S2>>;

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<common_pair<F1, S1>, std::pair<F2, S2>>;

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<common_pair<F1, S1>, common_pair<F2, S2>>;

        // common_type for std::tuples
        template<typename ...Ts, typename ...Us>
        struct common_type<common_tuple<Ts...>, std::tuple<Us...>>;

        template<typename ...Ts, typename ...Us>
        struct common_type<std::tuple<Ts...>, common_tuple<Us...>>;

        template<typename ...Ts, typename ...Us>
        struct common_type<common_tuple<Ts...>, common_tuple<Us...>>;

        // A common reference for std::pairs
        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<common_pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>;

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<std::pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>;

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<common_pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>;

        // A common reference for std::tuples
        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<common_tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>;

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<std::tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>;

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<common_tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>;
        /// \endcond
    }
}

#endif
