/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
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
            struct _copy_cv_
            {
                using type = To;
            };
            template<typename From, typename To>
            struct _copy_cv_<From const, To>
            {
                using type = To const;
            };
            template<typename From, typename To>
            struct _copy_cv_<From volatile, To>
            {
                using type = To volatile;
            };
            template<typename From, typename To>
            struct _copy_cv_<From const volatile, To>
            {
                using type = To const volatile;
            };
            template<typename From, typename To>
            using _copy_cv = meta::_t<_copy_cv_<From, To>>;

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename U, typename = void>
            struct _builtin_common;

            template<typename T, typename U>
            using _builtin_common_t = meta::_t<_builtin_common<T, U>>;

            template<typename T, typename U>
            using _cond_res = decltype(true ? std::declval<T>() : std::declval<U>());

            template<typename T, typename U, typename R = _builtin_common_t<T &, U &>>
            using _rref_res =
                meta::if_<std::is_reference<R>, meta::_t<std::remove_reference<R>> &&, R>;

            template<class T, class U>
            using _lref_res = _cond_res<_copy_cv<T, U> &, _copy_cv<U, T> &>;

        #if !defined(__GNUC__) || defined(__clang__)
            template<typename T, typename U, typename /*= void*/>
            struct _builtin_common
              : meta::lazy::let<
                    meta::defer<decay_t, meta::defer<_cond_res, as_cref_t<T>, as_cref_t<U>>>>
            {};
            template<typename T, typename U>
            struct _builtin_common<T &&, U &&, meta::if_<meta::and_<
                detail::is_convertible<T, _rref_res<T, U>>,
                detail::is_convertible<U, _rref_res<T, U>>>>>
            {
                using type = _rref_res<T, U>;
            };
            template<typename T, typename U>
            struct _builtin_common<T &, U &>
              : meta::defer<_lref_res, T, U>
            {};
            template<typename T, typename U>
            struct _builtin_common<T &, U &&, meta::if_<
                detail::is_convertible<U, _builtin_common_t<T &, U const &>>>>
              : _builtin_common<T &, U const &>
            {};
            template<typename T, typename U>
            struct _builtin_common<T &&, U &>
              : _builtin_common<U &, T &&>
            {};
        #else
            template<typename T, typename U, typename = void>
            struct _builtin_common_
            {};
            template<typename T, typename U>
            struct _builtin_common_<T, U, meta::void_<_cond_res<as_cref_t<T>, as_cref_t<U>>>>
              : std::decay<_cond_res<as_cref_t<T>, as_cref_t<U>>>
            {};
            template<typename T, typename U, typename /*= void*/>
            struct _builtin_common
              : _builtin_common_<T, U>
            {};
            template<typename T, typename U, typename = void>
            struct _builtin_common_rr
              : _builtin_common_<T &&, U &&>
            {};
            template<typename T, typename U>
            struct _builtin_common_rr<T, U, meta::if_<meta::and_<
                detail::is_convertible<T, _rref_res<T, U>>,
                detail::is_convertible<U, _rref_res<T, U>>>>>
            {
                using type = _rref_res<T, U>;
            };
            template<typename T, typename U>
            struct _builtin_common<T &&, U &&>
              : _builtin_common_rr<T, U>
            {};
            template<typename T, typename U>
            struct _builtin_common<T &, U &>
              : meta::defer<_lref_res, T, U>
            {};
            template<typename T, typename U, typename = void>
            struct _builtin_common_lr
              : _builtin_common_<T &, T &&>
            {};
            template<typename T, typename U>
            struct _builtin_common_lr<T, U, meta::if_<
                detail::is_convertible<U, _builtin_common_t<T &, U const &>>>>
              : _builtin_common<T &, U const &>
            {};
            template<typename T, typename U>
            struct _builtin_common<T &, U &&>
              : _builtin_common_lr<T, U>
            {};
            template<typename T, typename U>
            struct _builtin_common<T &&, U &>
              : _builtin_common<U &, T &&>
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

        template<typename T, typename U>
        struct common_type<T, U>
          : meta::if_c<
                ( std::is_same<detail::decay_t<T>, T>::value &&
                  std::is_same<detail::decay_t<U>, U>::value ),
                meta::defer<detail::_builtin_common_t, T, U>,
                common_type<detail::decay_t<T>, detail::decay_t<U>>>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_type<T, U, Vs...>
          : meta::lazy::fold<meta::list<U, Vs...>, T, meta::quote<common_type_t>>
        {};

        /// @}

        /// \addtogroup group-utility Utility
        /// @{
        ///

        /// Users can specialize this to hook the \c CommonReference concept.
        /// \sa `common_reference`
        template<typename T, typename U, typename TQual, typename UQual>
        struct basic_common_reference
        {};

        /// \cond
        namespace detail
        {
            using _rref =
                meta::quote_trait<std::add_rvalue_reference>;
            using _lref =
                meta::quote_trait<std::add_lvalue_reference>;

            template<typename T>
            struct _xref
            {
                using type = meta::quote_trait<meta::id>;
            };
            template<typename T>
            struct _xref<T &&>
            {
                using type = meta::compose<_rref, meta::_t<_xref<T>>>;
            };
            template<typename T>
            struct _xref<T &>
            {
                using type = meta::compose<_lref, meta::_t<_xref<T>>>;
            };
            template<typename T>
            struct _xref<T const>
            {
                using type = meta::quote_trait<std::add_const>;
            };
            template<typename T>
            struct _xref<T volatile>
            {
                using type = meta::quote_trait<std::add_volatile>;
            };
            template<typename T>
            struct _xref<T const volatile>
            {
                using type = meta::quote_trait<std::add_cv>;
            };

            template<typename T, typename U>
            using _basic_common_reference =
                basic_common_reference<
                    uncvref_t<T>,
                    uncvref_t<U>,
                    meta::_t<_xref<T>>,
                    meta::_t<_xref<U>>>;

            template<typename T, typename U, typename = void>
            struct _common_reference2
              : meta::if_<
                    meta::is_trait<_basic_common_reference<T, U>>,
                    _basic_common_reference<T, U>,
                    common_type<T, U>>
            {};

            template<typename T, typename U>
            struct _common_reference2<T, U, meta::if_<std::is_reference<_builtin_common_t<T, U>>>>
              : _builtin_common<T, U>
            {};
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

        template<typename T, typename U>
        struct common_reference<T, U>
          : detail::_common_reference2<T, U>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_reference<T, U, Vs...>
          : meta::lazy::fold<meta::list<U, Vs...>, T, meta::quote<common_reference_t>>
        {};

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
