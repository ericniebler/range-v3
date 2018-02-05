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

#ifndef RANGES_V3_UTILITY_COMMON_TUPLE_HPP
#define RANGES_V3_UTILITY_COMMON_TUPLE_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename ...Us, typename Tup, std::size_t...Is>
            std::tuple<Us...> to_std_tuple(Tup && tup, meta::index_sequence<Is...>)
            {
                return std::tuple<Us...>{std::get<Is>(static_cast<Tup&&>(tup))...};
            }
        }
        /// \endcond

        template<typename ...Ts>
        struct common_tuple
          : std::tuple<Ts...>
        {
        private:
            template<typename That, std::size_t...Is>
            common_tuple(That && that, meta::index_sequence<Is...>)
              : std::tuple<Ts...>{std::get<Is>(static_cast<That&&>(that))...}
            {}
            struct element_assign_
            {
                template<typename T, typename U>
                int operator()(T &t, U &&u) const
                {
                    t = static_cast<U&&>(u);
                    return 0;
                }
            };
        public:
            // Construction
            CONCEPT_requires(meta::and_c<(bool) DefaultConstructible<Ts>()...>::value)()
            common_tuple()
                noexcept(meta::and_c<std::is_nothrow_default_constructible<Ts>::value...>::value)
              : std::tuple<Ts...>{}
            {}
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Constructible<Ts, Us>()...>::value)
            (explicit) common_tuple(Us &&... us)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us>::value...>::value)
              : std::tuple<Ts...>{static_cast<Us&&>(us)...}
            {}
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Constructible<Ts, Us &>()...>::value)()
            common_tuple(std::tuple<Us...> &that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &>::value...>::value)
              : common_tuple(that, meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Constructible<Ts, Us const &>()...>::value)()
            common_tuple(std::tuple<Us...> const &that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us const &>::value...>::value)
              : common_tuple(that, meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Constructible<Ts, Us>()...>::value)()
            common_tuple(std::tuple<Us...> &&that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us>::value...>::value)
              : common_tuple(std::move(that), meta::make_index_sequence<sizeof...(Ts)>{})
            {}

            std::tuple<Ts...> & base() noexcept
            {
                return *this;
            }
            std::tuple<Ts...> const & base() const noexcept
            {
                return *this;
            }

            // Assignment
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Assignable<Ts &, Us &>()...>::value)
            (common_tuple &)operator=(std::tuple<Us...> &that)
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts &, Us &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Assignable<Ts &, Us const &>()...>::value)
            (common_tuple &)operator=(std::tuple<Us...> const & that)
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts &, Us const &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Assignable<Ts &, Us>()...>::value)
            (common_tuple &)operator=(std::tuple<Us...> &&that)
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts &, Us>::value...>::value)
            {
                (void)tuple_transform(base(), std::move(that), element_assign_{});
                return *this;
            }

            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Assignable<Ts const &, Us &>()...>::value)
            (common_tuple const &)operator=(std::tuple<Us...> &that) const
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts const &, Us &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Assignable<Ts const &, Us const &>()...>::value)
            (common_tuple const &)operator=(std::tuple<Us...> const & that) const
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts const &, Us const &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            CONCEPT_template(typename...Us)(
                requires meta::and_c<(bool) Assignable<Ts const &, Us &&>()...>::value)
            (common_tuple const &)operator=(std::tuple<Us...> &&that) const
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts const &, Us &&>::value...>::value)
            {
                (void)tuple_transform(base(), std::move(that), element_assign_{});
                return *this;
            }

            // Conversion
            CONCEPT_template(typename ...Us)(
                requires meta::and_c<(bool) Constructible<Us, Ts &>()...>::value)()
            operator std::tuple<Us...> () &
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts &>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(*this, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            CONCEPT_template(typename ...Us)(
                requires meta::and_c<(bool) Constructible<Us, Ts const &>()...>::value)()
            operator std::tuple<Us...> () const &
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts const &>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(*this, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            CONCEPT_template(typename ...Us)(
                requires meta::and_c<(bool) Constructible<Us, Ts>()...>::value)()
            operator std::tuple<Us...> () &&
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(std::move(*this), meta::make_index_sequence<sizeof...(Ts)>{});
            }
        };

        // Logical operators
#define LOGICAL_OP(OP, CONCEPT)\
        CONCEPT_template(typename...Ts, typename...Us)(\
            requires meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)\
        (bool) operator OP(common_tuple<Ts...> const &a, common_tuple<Us...> const &b)\
        {\
            return a.base() OP b.base();\
        }\
        CONCEPT_template(typename...Ts, typename...Us)(\
            requires meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)\
        (bool) operator OP(std::tuple<Ts...> const &a, common_tuple<Us...> const &b)\
        {\
            return a OP b.base();\
        }\
        CONCEPT_template(typename...Ts, typename...Us)(\
            requires meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)\
        (bool) operator OP(common_tuple<Ts...> const &a, std::tuple<Us...> const &b)\
        {\
            return a.base() OP b;\
        }\
        /**/
        LOGICAL_OP(==, EqualityComparableWith)
        LOGICAL_OP(!=, EqualityComparableWith)
        LOGICAL_OP(<, StrictTotallyOrderedWith)
        LOGICAL_OP(<=, StrictTotallyOrderedWith)
        LOGICAL_OP(>, StrictTotallyOrderedWith)
        LOGICAL_OP(>=, StrictTotallyOrderedWith)
#undef LOGICAL_OP

        struct make_common_tuple_fn
        {
            template<typename ...Args>
            common_tuple<bind_element_t<Args>...> operator()(Args &&... args) const
                noexcept(meta::and_c<
                    std::is_nothrow_constructible<
                        bind_element_t<Args>,
                        unwrap_reference_t<Args>>::value...>::value)
            {
                return common_tuple<bind_element_t<Args>...>{
                    unwrap_reference(static_cast<Args&&>(args))...};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_common_tuple_fn`
        RANGES_INLINE_VARIABLE(make_common_tuple_fn, make_common_tuple)

        template<typename F, typename S>
        struct common_pair
          : std::pair<F, S>
        {
        private:
            std::pair<F, S> const & base() const noexcept
            {
                return *this;
            }
        public:
            // Construction
            CONCEPT_requires(DefaultConstructible<F>() && DefaultConstructible<S>())()
            common_pair()
                noexcept(std::is_nothrow_default_constructible<F>::value &&
                    std::is_nothrow_default_constructible<S>::value)
              : std::pair<F, S>{}
            {}
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F, F2>() && Constructible<S, S2>())()
            common_pair(F2 &&f2, S2 &&s2)
                noexcept(std::is_nothrow_constructible<F, F2>::value &&
                    std::is_nothrow_constructible<S, S2>::value)
              : std::pair<F, S>{static_cast<F2&&>(f2), static_cast<S2&&>(s2)}
            {}
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F, F2 &>() && Constructible<S, S2 &>())()
            common_pair(std::pair<F2, S2> &that)
                noexcept(std::is_nothrow_constructible<F, F2 &>::value &&
                    std::is_nothrow_constructible<S, S2 &>::value)
              : std::pair<F, S>{that.first, that.second}
            {}
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F, F2 const &>() && Constructible<S, S2 const &>())()
            common_pair(std::pair<F2, S2> const &that)
                noexcept(std::is_nothrow_constructible<F, F2 const &>::value &&
                    std::is_nothrow_constructible<S, S2 const &>::value)
              : std::pair<F, S>{that.first, that.second}
            {}
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F, F2>() && Constructible<S, S2>())()
            common_pair(std::pair<F2, S2> &&that)
                noexcept(std::is_nothrow_constructible<F, F2>::value &&
                    std::is_nothrow_constructible<S, S2>::value)
              : std::pair<F, S>{std::forward<F2>(that.first), std::forward<S2>(that.second)}
            {}

            // Conversion
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F2, F &>() && Constructible<S2, S &>())()
            operator std::pair<F2, S2> () &
                noexcept(std::is_nothrow_constructible<F2, F &>::value &&
                    std::is_nothrow_constructible<S2, S &>::value)
            {
                return {this->first, this->second};
            }
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F2, F const &>() && Constructible<S2, S const &>())()
            operator std::pair<F2, S2> () const &
                noexcept(std::is_nothrow_constructible<F2, F const &>::value &&
                    std::is_nothrow_constructible<S2, S const &>::value)
            {
                return {this->first, this->second};
            }
            CONCEPT_template(typename F2, typename S2)(
                requires Constructible<F2, F>() && Constructible<S2, S>())()
            operator std::pair<F2, S2> () &&
                noexcept(std::is_nothrow_constructible<F2, F>::value &&
                    std::is_nothrow_constructible<S2, S>::value)
            {
                return {std::forward<F>(this->first), std::forward<S>(this->second)};
            }

            // Assignment
            CONCEPT_template(typename F2, typename S2)(
                requires Assignable<F &, F2 &>() && Assignable<S &, S2 &>())
            (common_pair &)operator=(std::pair<F2, S2> &that)
                noexcept(std::is_nothrow_assignable<F &, F2 &>::value &&
                         std::is_nothrow_assignable<S &, S2 &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            CONCEPT_template(typename F2, typename S2)(
                requires Assignable<F &, F2 const &>() && Assignable<S &, S2 const &>())
            (common_pair &)operator=(std::pair<F2, S2> const & that)
                noexcept(std::is_nothrow_assignable<F &, F2 const &>::value &&
                         std::is_nothrow_assignable<S &, S2 const &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            CONCEPT_template(typename F2, typename S2)(
                requires Assignable<F &, F2>() && Assignable<S &, S2>())
            (common_pair &)operator=(std::pair<F2, S2> &&that)
                noexcept(std::is_nothrow_assignable<F &, F2>::value &&
                         std::is_nothrow_assignable<S &, S2>::value)
            {
                this->first = static_cast<F2&&>(that.first);
                this->second = static_cast<S2&&>(that.second);
                return *this;
            }

            CONCEPT_template(typename F2, typename S2)(
                requires Assignable<F const &, F2 &>() && Assignable<S const &, S2 &>())
            (common_pair const &)operator=(std::pair<F2, S2> &that) const
                noexcept(std::is_nothrow_assignable<F const &, F2 &>::value &&
                         std::is_nothrow_assignable<S const &, S2 &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            CONCEPT_template(typename F2, typename S2)(
                requires Assignable<F const &, F2 const &>() && Assignable<S const &, S2 const &>())
            (common_pair const &)operator=(std::pair<F2, S2> const & that) const
                noexcept(std::is_nothrow_assignable<F const &, F2 const &>::value &&
                         std::is_nothrow_assignable<S const &, S2 const &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            CONCEPT_template(typename F2, typename S2)(
                requires Assignable<F const &, F2 &&>() && Assignable<S const &, S2 &&>())
            (common_pair const &)operator=(std::pair<F2, S2> &&that) const
                noexcept(std::is_nothrow_assignable<F const &, F2 &&>::value &&
                         std::is_nothrow_assignable<S const &, S2 &&>::value)
            {
                this->first = static_cast<F2&&>(that.first);
                this->second = static_cast<S2&&>(that.second);
                return *this;
            }
        };

        // Logical operators
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(
            requires EqualityComparableWith<F1, F2>() && EqualityComparableWith<S1, S2>())
        (bool) operator ==(common_pair<F1, S1> const &a, common_pair<F2, S2> const &b)
        {
            return a.first == b.first && a.second == b.second;
        }
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(
            requires EqualityComparableWith<F1, F2>() && EqualityComparableWith<S1, S2>())
        (bool) operator ==(common_pair<F1, S1> const &a, std::pair<F2, S2> const &b)
        {
            return a.first == b.first && a.second == b.second;
        }
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(
            requires EqualityComparableWith<F1, F2>() && EqualityComparableWith<S1, S2>())
        (bool) operator ==(std::pair<F1, S1> const &a, common_pair<F2, S2> const &b)
        {
            return a.first == b.first && a.second == b.second;
        }
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(
            requires StrictTotallyOrderedWith<F1, F2>() && StrictTotallyOrderedWith<S1, S2>())
        (bool) operator <(common_pair<F1, S1> const &a, common_pair<F2, S2> const &b)
        {
            return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
        }
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(
            requires StrictTotallyOrderedWith<F1, F2>() && StrictTotallyOrderedWith<S1, S2>())
        (bool) operator <(std::pair<F1, S1> const &a, common_pair<F2, S2> const &b)
        {
            return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
        }
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(
            requires StrictTotallyOrderedWith<F1, F2>() && StrictTotallyOrderedWith<S1, S2>())
        (bool) operator <(common_pair<F1, S1> const &a, std::pair<F2, S2> const &b)
        {
            return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
        }
#define LOGICAL_OP(OP, CONCEPT, RET)\
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(\
            requires CONCEPT<F1, F2>() && CONCEPT<S1, S2>())\
        (bool) operator OP(common_pair<F1, S1> const &a, common_pair<F2, S2> const &b)\
        {\
            return RET;\
        }\
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(\
            requires CONCEPT<F1, F2>() && CONCEPT<S1, S2>())\
        (bool) operator OP(std::pair<F1, S1> const &a, common_pair<F2, S2> const &b)\
        {\
            return RET;\
        }\
        CONCEPT_template(typename F1, typename S1, typename F2, typename S2)(\
            requires CONCEPT<F1, F2>() && CONCEPT<S1, S2>())\
        (bool) operator OP(common_pair<F1, S1> const &a, std::pair<F2, S2> const &b)\
        {\
            return RET;\
        }\
        /**/
        LOGICAL_OP(!=, EqualityComparableWith, !(a == b))
        LOGICAL_OP(<=, StrictTotallyOrderedWith, !(b < a))
        LOGICAL_OP(>, StrictTotallyOrderedWith, (b < a))
        LOGICAL_OP(>=, StrictTotallyOrderedWith, !(a < b))
#undef LOGICAL_OP

        struct make_common_pair_fn
        {
            template<typename First, typename Second,
                typename F = bind_element_t<First>,
                typename S = bind_element_t<Second>>
            common_pair<F, S> operator()(First && f, Second && s) const
                noexcept(std::is_nothrow_constructible<F, unwrap_reference_t<First>>::value &&
                    std::is_nothrow_constructible<F, unwrap_reference_t<Second>>::value)
            {
                return {
                    unwrap_reference(static_cast<First&&>(f)),
                    unwrap_reference(static_cast<Second&&>(s))};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_common_pair_fn`
        RANGES_INLINE_VARIABLE(make_common_pair_fn, make_common_pair)

        /// \cond
        namespace detail
        {
            template<typename, typename, typename, typename = void>
            struct common_type_tuple_like
            {};

            template<template<typename...> class T0, typename ...Ts,
                     template<typename...> class T1, typename ...Us,
                     typename TupleLike>
            struct common_type_tuple_like<T0<Ts...>, T1<Us...>, TupleLike,
                meta::if_c<sizeof...(Ts) == sizeof...(Us)>>
              : meta::lazy::let<
                    meta::lazy::invoke<TupleLike, meta::lazy::_t<common_type<Ts, Us>>...>>
            {};

            template<typename T, typename U>
            using make_common_pair =
                meta::if_<
                    meta::or_<std::is_reference<T>, std::is_reference<U>>,
                    common_pair<T, U>,
                    std::pair<T, U>>;

            template<typename...Ts>
            using make_common_tuple =
                meta::if_<
                    meta::any_of<meta::list<Ts...>, meta::quote<std::is_reference>>,
                    common_tuple<Ts...>,
                    std::tuple<Ts...>>;

            template<typename, typename, typename, typename = void>
            struct common_ref_tuple_like
            {};

            template<template<typename...> class T0, typename ...Ts,
                     template<typename...> class T1, typename ...Us, typename TupleLike>
            struct common_ref_tuple_like<T0<Ts...>, T1<Us...>, TupleLike,
                meta::if_c<sizeof...(Ts) == sizeof...(Us)>>
              : meta::lazy::let<
                    meta::lazy::invoke<TupleLike, meta::lazy::_t<common_reference<Ts, Us>>...>>
            {};
        }
        /// \endcond
    }
}

/// \cond
namespace concepts
{
    // common_type for pairs
    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<std::pair<F1, S1>, ranges::common_pair<F2, S2>>
      : ranges::detail::common_type_tuple_like<std::pair<F1, S1>, ranges::common_pair<F2, S2>,
            meta::quote<ranges::detail::make_common_pair>>
    {};
    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<ranges::common_pair<F1, S1>, std::pair<F2, S2>>
      : ranges::detail::common_type_tuple_like<ranges::common_pair<F1, S1>, std::pair<F2, S2>,
            meta::quote<ranges::detail::make_common_pair>>
    {};
    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<ranges::common_pair<F1, S1>, ranges::common_pair<F2, S2>>
      : ranges::detail::common_type_tuple_like<ranges::common_pair<F1, S1>, ranges::common_pair<F2, S2>,
            meta::quote<ranges::common_pair>>
    {};
    // common_type for tuples
    template<typename ...Ts, typename ...Us>
    struct common_type<ranges::common_tuple<Ts...>, std::tuple<Us...>>
      : ranges::detail::common_type_tuple_like<ranges::common_tuple<Ts...>, std::tuple<Us...>,
            meta::quote<ranges::detail::make_common_tuple>>
    {};
    template<typename ...Ts, typename ...Us>
    struct common_type<std::tuple<Ts...>, ranges::common_tuple<Us...>>
      : ranges::detail::common_type_tuple_like<std::tuple<Ts...>, ranges::common_tuple<Us...>,
            meta::quote<ranges::detail::make_common_tuple>>
    {};
    template<typename ...Ts, typename ...Us>
    struct common_type<ranges::common_tuple<Ts...>, ranges::common_tuple<Us...>>
      : ranges::detail::common_type_tuple_like<ranges::common_tuple<Ts...>, ranges::common_tuple<Us...>,
            meta::quote<ranges::common_tuple>>
    {};

    // common reference for pairs
    template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
    struct basic_common_reference<ranges::common_pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>
      : ranges::detail::common_ref_tuple_like<
            ranges::common_pair<meta::invoke<Qual1, F1>, meta::invoke<Qual1, S1>>,
            std::pair<meta::invoke<Qual2, F2>, meta::invoke<Qual2, S2>>,
            meta::quote<ranges::detail::make_common_pair>>
    {};
    template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
    struct basic_common_reference<std::pair<F1, S1>, ranges::common_pair<F2, S2>, Qual1, Qual2>
      : ranges::detail::common_ref_tuple_like<
            std::pair<meta::invoke<Qual1, F1>, meta::invoke<Qual1, S1>>,
            ranges::common_pair<meta::invoke<Qual2, F2>, meta::invoke<Qual2, S2>>,
            meta::quote<ranges::detail::make_common_pair>>
    {};
    template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
    struct basic_common_reference<ranges::common_pair<F1, S1>, ranges::common_pair<F2, S2>, Qual1, Qual2>
      : ranges::detail::common_ref_tuple_like<
            ranges::common_pair<meta::invoke<Qual1, F1>, meta::invoke<Qual1, S1>>,
            ranges::common_pair<meta::invoke<Qual2, F2>, meta::invoke<Qual2, S2>>,
            meta::quote<ranges::common_pair>>
    {};
    // common reference for tuples
    template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
    struct basic_common_reference<ranges::common_tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>
      : ranges::detail::common_ref_tuple_like<
            ranges::common_tuple<meta::invoke<Qual1, Ts>...>,
            std::tuple<meta::invoke<Qual2, Us>...>,
            meta::quote<ranges::detail::make_common_tuple>>
    {};
    template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
    struct basic_common_reference<std::tuple<Ts...>, ranges::common_tuple<Us...>, Qual1, Qual2>
      : ranges::detail::common_ref_tuple_like<
            std::tuple<meta::invoke<Qual1, Ts>...>,
            ranges::common_tuple<meta::invoke<Qual2, Us>...>,
            meta::quote<ranges::detail::make_common_tuple>>
    {};
    template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
    struct basic_common_reference<ranges::common_tuple<Ts...>, ranges::common_tuple<Us...>, Qual1, Qual2>
      : ranges::detail::common_ref_tuple_like<
            ranges::common_tuple<meta::invoke<Qual1, Ts>...>,
            ranges::common_tuple<meta::invoke<Qual2, Us>...>,
            meta::quote<ranges::common_tuple>>
    {};
}
/// \endcond

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std
{
    template<typename First, typename Second>
    struct tuple_size< ::ranges::v3::common_pair<First, Second>>
      : std::integral_constant<size_t, 2>
    {};

    template<typename First, typename Second>
    struct tuple_element<0, ::ranges::v3::common_pair<First, Second>>
    {
        using type = First;
    };

    template<typename First, typename Second>
    struct tuple_element<1, ::ranges::v3::common_pair<First, Second>>
    {
        using type = Second;
    };

    template<typename... Ts>
    struct tuple_size< ::ranges::v3::common_tuple<Ts...>>
      : std::integral_constant<size_t, sizeof...(Ts)>
    {};

    template<size_t N, typename... Ts>
    struct tuple_element<N, ::ranges::v3::common_tuple<Ts...>>
      : tuple_element<N, tuple<Ts...>>
    {};
}

RANGES_DIAGNOSTIC_POP

#endif
