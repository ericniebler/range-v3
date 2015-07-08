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
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>

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
            template<typename T, typename U, typename = void>
            struct builtin_common;

            template<typename T, typename U>
            using builtin_common_t = meta::eval<builtin_common<T, U>>;

            template<typename T, typename U>
            using lazy_builtin_common_t = meta::defer<builtin_common_t, T, U>;

            template<typename T, typename U>
            using cond_res_t = decltype(true ? std::declval<T>() : std::declval<U>());

        #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
            template<typename T, typename U, typename /*= void*/>
            struct builtin_common
              : meta::lazy::let<
                    meta::defer<decay_t, meta::defer<cond_res_t, as_cref_t<T>, as_cref_t<U>>>>
            {};

            template<typename T, typename U, typename R = builtin_common_t<T &, U &>>
            using _rref_res =
                meta::if_<std::is_reference<R>, meta::eval<std::remove_reference<R>> &&, R>;

            template<typename T, typename U>
            struct builtin_common<T &&, U &&, meta::if_<meta::and_<
                std::is_convertible<T &&, _rref_res<T, U>>,
                std::is_convertible<U &&, _rref_res<T, U>>>>>
            {
                using type = _rref_res<T, U>;
            };

            template <class T, class U>
            using _lref_res = cond_res_t<copy_cv_t<T, U> &, copy_cv_t<U, T> &>;

            template<typename T, typename U>
            struct builtin_common<T &, U &>
              : meta::defer<_lref_res, T, U>
            {};
            template<typename T, typename U>
            struct builtin_common<T &, U &&, meta::if_<
                std::is_convertible<U &&, builtin_common_t<T &, U const &>>>>
              : builtin_common<T &, U const &>
            {};
            template<typename T, typename U>
            struct builtin_common<T &&, U &>
              : builtin_common<U &, T &&>
            {};
        #else
            template<typename T, typename U, typename = void>
            struct builtin_common_
            {};
            template<typename T, typename U>
            struct builtin_common_<T, U, meta::void_<cond_res_t<as_cref_t<T>, as_cref_t<U>>>>
              : std::decay<cond_res_t<as_cref_t<T>, as_cref_t<U>>>
            {};

            template<typename T, typename U, typename /*= void*/>
            struct builtin_common
              : builtin_common_<T, U>
            {};

            template<typename T, typename U, typename R = builtin_common_t<T &, U &>>
            using _rref_res =
                meta::if_<std::is_reference<R>, meta::eval<std::remove_reference<R>> &&, R>;

            template<typename T, typename U, typename = void>
            struct builtin_common_rr
              : builtin_common_<T &&, U &&>
            {};
            template<typename T, typename U>
            struct builtin_common_rr<T, U, meta::if_<meta::and_<
                std::is_convertible<T &&, _rref_res<T, U>>,
                std::is_convertible<U &&, _rref_res<T, U>>>>>
            {
                using type = _rref_res<T, U>;
            };
            template<typename T, typename U>
            struct builtin_common<T &&, U &&>
              : builtin_common_rr<T, U>
            {};

            template <class T, class U>
            using _lref_res = cond_res_t<copy_cv_t<T, U> &, copy_cv_t<U, T> &>;

            template<typename T, typename U>
            struct builtin_common<T &, U &>
              : meta::defer<_lref_res, T, U>
            {};
            template<typename T, typename U, typename = void>
            struct builtin_common_lr
              : builtin_common_<T &, T &&>
            {};
            template<typename T, typename U>
            struct builtin_common_lr<T, U, meta::if_<
                std::is_convertible<U &&, builtin_common_t<T &, U const &>>>>
              : builtin_common<T &, U const &>
            {};
            template<typename T, typename U>
            struct builtin_common<T &, U &&>
              : builtin_common_lr<T, U>
            {};
            template<typename T, typename U>
            struct builtin_common<T &&, U &>
              : builtin_common<U &, T &&>
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
                detail::lazy_builtin_common_t<T, U>,
                common_type<detail::decay_t<T>, detail::decay_t<U>>>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_type<T, U, Vs...>
          : meta::lazy::fold<meta::list<U, Vs...>, T, meta::quote<common_type_t>>
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
                meta::compose<rvalue_ref_t, meta::quote_trait<std::add_const>>;

            using lvalue_ref_t =
                meta::quote_trait<std::add_lvalue_reference>;

            using const_lvalue_ref_t =
                meta::compose<lvalue_ref_t, meta::quote_trait<std::add_const>>;
        }

        /// Users can specialize this to hook the \c CommonReference concept.
        /// \sa `common_reference`
        template<typename T, typename U, typename TQual, typename UQual>
        struct basic_common_reference
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
            using basic_common_reference_ =
                basic_common_reference<
                    uncvref_t<T>,
                    uncvref_t<U>,
                    meta::eval<transform_reference<T>>,
                    meta::eval<transform_reference<U>>>;

        #if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
        #else
            template<typename T, typename U, typename Enable = void>
            struct common_reference_if
              : basic_common_reference_<T, U>
            {};

            template<typename T, typename U>
            struct common_reference_if<T, U, meta::void_<builtin_common_t<T, U>>>
              : meta::if_<
                    meta::or_<
                        std::is_reference<builtin_common_t<T, U>>,
                        meta::not_<meta::has_type<basic_common_reference_<T, U>>>>,
                    meta::id<builtin_common_t<T, U>>,
                    basic_common_reference_<T, U>>
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
        /// \sa `basic_common_reference`
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
                meta::let<meta::lazy::and_<
                    meta::is_valid<detail::lazy_builtin_common_t<T, U>>,
                    meta::lazy::or_<
                        std::is_reference<detail::lazy_builtin_common_t<T, U>>,
                        meta::not_<meta::has_type<detail::basic_common_reference_<T, U>>>>>>,
                detail::lazy_builtin_common_t<T, U>,
                detail::basic_common_reference_<T, U>>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_reference<T, U, Vs...>
          : meta::lazy::fold<meta::list<U, Vs...>, T, meta::quote<common_reference_t>>
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
        struct basic_common_reference<common_pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>;

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct basic_common_reference<std::pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>;

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct basic_common_reference<common_pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>;

        // A common reference for std::tuples
        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct basic_common_reference<common_tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>;

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct basic_common_reference<std::tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>;

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct basic_common_reference<common_tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>;
        /// \endcond
    }
}

#endif
