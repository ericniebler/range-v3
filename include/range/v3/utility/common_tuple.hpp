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

#ifndef RANGES_V3_UTILITY_COMMON_TUPLE_HPP
#define RANGES_V3_UTILITY_COMMON_TUPLE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
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
            std::tuple<Us...> to_std_tuple(Tup && tup, index_sequence<Is...>)
            {
                return std::tuple<Us...>{std::get<Is>(std::forward<Tup>(tup))...};
            }
        }
        /// \endcond

        template<typename ...Ts>
        struct common_tuple
          : std::tuple<Ts...>
        {
        private:
            template<typename That, std::size_t...Is>
            common_tuple(That && that, index_sequence<Is...>)
              : std::tuple<Ts...>{std::get<Is>(std::forward<That>(that))...}
            {}
            std::tuple<Ts...> & base() noexcept
            {
                return *this;
            }
            std::tuple<Ts...> const & base() const noexcept
            {
                return *this;
            }
            struct element_assign_
            {
                template<typename T, typename U>
                int operator()(T &t, U &&u) const
                {
                    t = std::forward<U>(u);
                    return 0;
                }
            };
        public:
            // Construction
            CONCEPT_REQUIRES(meta::and_c<(bool) DefaultConstructible<Ts>()...>::value)
            common_tuple()
                noexcept(meta::and_c<std::is_nothrow_default_constructible<Ts>::value...>::value)
              : std::tuple<Ts...>{}
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &&>()...>::value)>
            explicit common_tuple(Us &&... us)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &&>::value...>::value)
              : std::tuple<Ts...>{std::forward<Us>(us)...}
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &>()...>::value)>
            common_tuple(std::tuple<Us...> &that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &>::value...>::value)
              : common_tuple(that, make_index_sequence<sizeof...(Ts)>{})
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us const &>()...>::value)>
            common_tuple(std::tuple<Us...> const &that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us const &>::value...>::value)
              : common_tuple(that, make_index_sequence<sizeof...(Ts)>{})
            {}
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &&>()...>::value)>
            common_tuple(std::tuple<Us...> &&that)
                noexcept(meta::and_c<std::is_nothrow_constructible<Ts, Us &&>::value...>::value)
              : common_tuple(std::move(that), make_index_sequence<sizeof...(Ts)>{})
            {}

            // Assignment
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Assignable<Ts &, Us &>()...>::value)>
            common_tuple &operator=(std::tuple<Us...> &that)
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts &, Us &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Assignable<Ts &, Us const &>()...>::value)>
            common_tuple &operator=(std::tuple<Us...> const & that)
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts &, Us const &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            template<typename...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Assignable<Ts &, Us &&>()...>::value)>
            common_tuple &operator=(std::tuple<Us...> &&that)
                noexcept(meta::and_c<std::is_nothrow_assignable<Ts &, Us &&>::value...>::value)
            {
                (void)tuple_transform(base(), std::move(that), element_assign_{});
                return *this;
            }

            // Conversion
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts &>()...>::value)>
            operator std::tuple<Us...> () &
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts &>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(*this, make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts const &>()...>::value)>
            operator std::tuple<Us...> () const &
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts const &>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(*this, make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Us,
                CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Us, Ts &&>()...>::value)>
            operator std::tuple<Us...> () &&
                noexcept(meta::and_c<std::is_nothrow_constructible<Us, Ts &&>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(std::move(*this), make_index_sequence<sizeof...(Ts)>{});
            }

        // Logical operators
#define LOGICAL_OP(OP, CONCEPT)\
            CONCEPT_REQUIRES(meta::and_c<(bool) CONCEPT<Ts>()...>::value)\
            friend bool operator OP(common_tuple const &a, common_tuple const &b)\
            {\
                return a.base() OP b.base();\
            }\
            template<typename...Us,\
                CONCEPT_REQUIRES_(meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)>\
            friend bool operator OP(common_tuple const &a, common_tuple<Us...> const &b)\
            {\
                return a.base() OP b.base();\
            }\
            template<typename...Us,\
                CONCEPT_REQUIRES_(meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)>\
            friend bool operator OP(common_tuple const &a, std::tuple<Us...> const &b)\
            {\
                return a.base() OP b;\
            }\
            template<typename...Us,\
                CONCEPT_REQUIRES_(meta::and_c<(bool) CONCEPT<Ts, Us>()...>::value)>\
            friend bool operator OP(std::tuple<Us...> const &a, common_tuple const &b)\
            {\
                return a OP b.base();\
            }\
            /**/
            LOGICAL_OP(==, EqualityComparable)
            LOGICAL_OP(!=, EqualityComparable)
            LOGICAL_OP(<, TotallyOrdered)
            LOGICAL_OP(<=, TotallyOrdered)
            LOGICAL_OP(>, TotallyOrdered)
            LOGICAL_OP(>=, TotallyOrdered)
#undef LOGICAL_OP
        };

        struct make_common_tuple_fn
        {
            using expects_wrapped_references = void;
            template<typename ...Args>
            common_tuple<bind_element_t<Args>...> operator()(Args &&... args) const
                noexcept(meta::and_c<
                    std::is_nothrow_constructible<
                        bind_element_t<Args>,
                        unwrap_reference_t<Args> >::value...>::value)
            {
                return common_tuple<bind_element_t<Args>...>{
                    unwrap_reference(std::forward<Args>(args))...};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_common_tuple_fn`
        namespace
        {
            constexpr auto&& make_common_tuple = static_const<make_common_tuple_fn>::value;
        }

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
            CONCEPT_REQUIRES(DefaultConstructible<F>() && DefaultConstructible<S>())
            common_pair()
                noexcept(std::is_nothrow_default_constructible<F>::value &&
                    std::is_nothrow_default_constructible<S>::value)
              : std::pair<F, S>{}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 &&>() && Constructible<S, S2 &&>())>
            common_pair(F2 &&f2, S2 &&s2)
                noexcept(std::is_nothrow_constructible<F, F2 &&>::value &&
                    std::is_nothrow_constructible<S, S2 &&>::value)
              : std::pair<F, S>{std::forward<F2>(f2), std::forward<S2>(s2)}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 &>() && Constructible<S, S2 &>())>
            common_pair(std::pair<F2, S2> &that)
                noexcept(std::is_nothrow_constructible<F, F2 &>::value &&
                    std::is_nothrow_constructible<S, S2 &>::value)
              : std::pair<F, S>{that.first, that.second}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 const &>() && Constructible<S, S2 const &>())>
            common_pair(std::pair<F2, S2> const &that)
                noexcept(std::is_nothrow_constructible<F, F2 const &>::value &&
                    std::is_nothrow_constructible<S, S2 const &>::value)
              : std::pair<F, S>{that.first, that.second}
            {}
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F, F2 &&>() && Constructible<S, S2 &&>())>
            common_pair(std::pair<F2, S2> &&that)
                noexcept(std::is_nothrow_constructible<F, F2 &&>::value &&
                    std::is_nothrow_constructible<S, S2 &&>::value)
              : std::pair<F, S>{std::forward<F2>(that.first), std::forward<S2>(that.second)}
            {}

            // Conversion
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F &>() && Constructible<S2, S &>())>
            operator std::pair<F2, S2> () &
                noexcept(std::is_nothrow_constructible<F2, F &>::value &&
                    std::is_nothrow_constructible<S2, S &>::value)
            {
                return {this->first, this->second};
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F const &>() && Constructible<S2, S const &>())>
            operator std::pair<F2, S2> () const &
                noexcept(std::is_nothrow_constructible<F2, F const &>::value &&
                    std::is_nothrow_constructible<S2, S const &>::value)
            {
                return {this->first, this->second};
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Constructible<F2, F &&>() && Constructible<S2, S &&>())>
            operator std::pair<F2, S2> () &&
                noexcept(std::is_nothrow_constructible<F2, F &&>::value &&
                    std::is_nothrow_constructible<S2, S &&>::value)
            {
                return {std::forward<F>(this->first), std::forward<S>(this->second)};
            }

            // Assignment
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Assignable<F &, F2 &>() && Assignable<S &, S2 &>())>
            common_pair &operator=(std::pair<F2, S2> &that)
                noexcept(std::is_nothrow_assignable<F &, F2 &>::value &&
                         std::is_nothrow_assignable<S &, S2 &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Assignable<F &, F2 const &>() && Assignable<S &, S2 const &>())>
            common_pair &operator=(std::pair<F2, S2> const & that)
                noexcept(std::is_nothrow_assignable<F &, F2 const &>::value &&
                         std::is_nothrow_assignable<S &, S2 const &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(Assignable<F &, F2 &&>() && Assignable<S &, S2 &&>())>
            common_pair &operator=(std::pair<F2, S2> &&that)
                noexcept(std::is_nothrow_assignable<F &, F2 &&>::value &&
                         std::is_nothrow_assignable<S &, S2 &&>::value)
            {
                this->first = std::forward<F2>(that.first);
                this->second = std::forward<S2>(that.second);
                return *this;
            }

            // Logical operators
            CONCEPT_REQUIRES(EqualityComparable<F>() && EqualityComparable<S>())
            friend bool operator ==(common_pair const &a, common_pair const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator ==(common_pair const &a, common_pair<F2, S2> const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator ==(common_pair const &a, std::pair<F2, S2> const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator ==(std::pair<F2, S2> const &a, common_pair const &b)
            {
                return a.first == b.first && a.second == b.second;
            }
            CONCEPT_REQUIRES(EqualityComparable<F>() && EqualityComparable<S>())
            friend bool operator <(common_pair const &a, common_pair const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator <(common_pair const &a, common_pair<F2, S2> const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator <(common_pair const &a, std::pair<F2, S2> const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
            template<typename F2, typename S2,
                CONCEPT_REQUIRES_(EqualityComparable<F, F2>() && EqualityComparable<S, S2>())>
            friend bool operator <(std::pair<F2, S2> const &a, common_pair const &b)
            {
                return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
            }
#define LOGICAL_OP(OP, CONCEPT, RET)\
            CONCEPT_REQUIRES(CONCEPT<F>() && CONCEPT<S>())\
            friend bool operator OP(common_pair const &a, common_pair const &b)\
            {\
                return RET;\
            }\
            template<typename F2, typename S2,\
                CONCEPT_REQUIRES_(CONCEPT<F, F2>() && CONCEPT<S, S2>())>\
            friend bool operator OP(common_pair const &a, common_pair<F2, S2> const &b)\
            {\
                return RET;\
            }\
            template<typename F2, typename S2,\
                CONCEPT_REQUIRES_(CONCEPT<F, F2>() && CONCEPT<S, S2>())>\
            friend bool operator OP(common_pair const &a, std::pair<F2, S2> const &b)\
            {\
                return RET;\
            }\
            template<typename F2, typename S2,\
                CONCEPT_REQUIRES_(CONCEPT<F, F2>() && CONCEPT<S, S2>())>\
            friend bool operator OP(std::pair<F2, S2> const &a, common_pair const &b)\
            {\
                return RET;\
            }\
            /**/
            LOGICAL_OP(!=, EqualityComparable, !(a == b))
            LOGICAL_OP(<=, TotallyOrdered, !(b < a))
            LOGICAL_OP(>, TotallyOrdered, (b < a))
            LOGICAL_OP(>=, TotallyOrdered, !(a < b))
#undef LOGICAL_OP
        };

        struct make_common_pair_fn
        {
            using expects_wrapped_references = void;
            template<typename First, typename Second,
                typename F = bind_element_t<First>,
                typename S = bind_element_t<Second>>
            common_pair<F, S> operator()(First && f, Second && s) const
                noexcept(std::is_nothrow_constructible<F, unwrap_reference_t<First>>::value &&
                    std::is_nothrow_constructible<F, unwrap_reference_t<Second>>::value)
            {
                return {
                    unwrap_reference(std::forward<First>(f)),
                    unwrap_reference(std::forward<Second>(s))};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_common_pair_fn`
        namespace
        {
            constexpr auto&& make_common_pair = static_const<make_common_pair_fn>::value;
        }

        /// \cond
        namespace detail
        {
            template<typename T, typename U, typename TupleLike>
            struct common_tuple_like;

            template<template<typename...> class T0, typename ...Ts,
                     template<typename...> class T1, typename ...Us,
                     typename TupleLike>
            struct common_tuple_like<T0<Ts...>, T1<Us...>, TupleLike>
              : meta::if_<
                    meta::and_<meta::has_type<common_type<Ts, Us> >...>,
                    meta::lazy_apply<
                        meta::compose<
                            meta::uncurry<TupleLike>,
                            meta::bind_back<meta::quote<meta::transform>, meta::quote<meta::eval> > >,
                        meta::list<common_type<Ts, Us>...> >,
                    meta::nil_>
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
        }

        // common_type for std::pairs
        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<std::pair<F1, S1>, common_pair<F2, S2>>
          : detail::common_tuple_like<std::pair<F1, S1>, common_pair<F2, S2>, meta::quote<std::pair>>
        {};

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<common_pair<F1, S1>, std::pair<F2, S2>>
          : detail::common_tuple_like<common_pair<F1, S1>, std::pair<F2, S2>, meta::quote<std::pair>>
        {};

        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<common_pair<F1, S1>, common_pair<F2, S2>>
          : detail::common_tuple_like<common_pair<F1, S1>, common_pair<F2, S2>, meta::quote<std::pair>>
        {};

        // common_type for std::tuples
        template<typename ...Ts, typename ...Us>
        struct common_type<common_tuple<Ts...>, std::tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<common_tuple<Ts...>, std::tuple<Us...>, meta::quote<std::tuple> >,
                meta::nil_>
        {};

        template<typename ...Ts, typename ...Us>
        struct common_type<std::tuple<Ts...>, common_tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<std::tuple<Ts...>, common_tuple<Us...>, meta::quote<std::tuple> >,
                meta::nil_>
        {};

        template<typename ...Ts, typename ...Us>
        struct common_type<common_tuple<Ts...>, common_tuple<Us...>>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like<common_tuple<Ts...>, common_tuple<Us...>, meta::quote<std::tuple> >,
                meta::nil_>
        {};

        namespace detail
        {
            template<typename T, typename U, typename TupleLike>
            struct common_tuple_like_ref;

            template<template<typename...> class T0, typename ...Ts,
                     template<typename...> class T1, typename ...Us, typename TupleLike>
            struct common_tuple_like_ref<T0<Ts...>, T1<Us...>, TupleLike>
              : meta::if_<
                    meta::and_<meta::has_type<common_reference<Ts, Us> >...>,
                    meta::lazy_apply<
                        meta::compose<
                            meta::uncurry<TupleLike>,
                            meta::bind_back<meta::quote<meta::transform>, meta::quote<meta::eval> > >,
                        meta::list<common_reference<Ts, Us>...> >,
                    meta::nil_>
            {};
        }

        // A common reference for std::pairs
        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<common_pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                common_pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                std::pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<detail::make_common_pair> >
        {};

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<std::pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                std::pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                common_pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<detail::make_common_pair> >
        {};

        template<typename F1, typename S1, typename F2, typename S2, typename Qual1, typename Qual2>
        struct common_reference_base<common_pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>
          : detail::common_tuple_like_ref<
                common_pair<meta::apply<Qual1, F1>, meta::apply<Qual1, S1> >,
                common_pair<meta::apply<Qual2, F2>, meta::apply<Qual2, S2> >,
                meta::quote<detail::make_common_pair> >
        {};

        // A common reference for std::tuples
        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<common_tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    common_tuple<meta::apply<Qual1, Ts>...>,
                    std::tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<detail::make_common_tuple> >,
                meta::nil_>
        {};

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<std::tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    std::tuple<meta::apply<Qual1, Ts>...>,
                    common_tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<detail::make_common_tuple> >,
                meta::nil_>
        {};

        template<typename ...Ts, typename ...Us, typename Qual1, typename Qual2>
        struct common_reference_base<common_tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>
          : meta::if_c<
                sizeof...(Ts) == sizeof...(Us),
                detail::common_tuple_like_ref<
                    common_tuple<meta::apply<Qual1, Ts>...>,
                    common_tuple<meta::apply<Qual2, Us>...>,
                    meta::quote<detail::make_common_tuple> >,
                meta::nil_>
        {};
        /// \endcond
    }
}

#endif
