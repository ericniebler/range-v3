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

#ifndef RANGES_V3_VIEW_ZIP_HPP
#define RANGES_V3_VIEW_ZIP_HPP

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            constexpr struct
            {
                template<typename T, typename U>
                bool operator()(T const &t, U const &u) const
                {
                    return static_cast<bool>(t == u);
                }
            } equal_to {};

            constexpr struct
            {
                template<typename T>
                auto operator()(T const & t) const noexcept(noexcept(*t)) -> decltype(*t)
                {
                    return *t;
                }
            } deref {};

            constexpr struct
            {
                template<typename T>
                void operator()(T & t) const
                {
                    --t;
                }
            } dec {};

            constexpr struct
            {
                template<typename T>
                void operator()(T & t) const
                {
                    ++t;
                }
            } inc {};

            constexpr struct
            {
                template<typename T, typename D>
                void operator()(T & t, D d) const
                {
                    advance(t, d);
                }
            } advance_ {};

            constexpr struct
            {
                template<typename T>
                auto operator()(T const &t, T const &u) const ->
                    decltype(u - t)
                {
                    return u - t;
                }
            } distance_to {};

            constexpr struct
            {
                template<typename T, typename U>
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? t : u)
                {
                    return t < u ? t : u;
                }
            } min_ {};

            constexpr struct
            {
                template<typename T, typename U>
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? u : t)
                {
                    return t < u ? u : t;
                }
            } max_ {};

            struct make_tuple_like_fn
            {
                template<typename ...Ts>
                std::tuple<Ts...> operator()(Ts &&...ts) const
                    noexcept(std::is_nothrow_move_constructible<std::tuple<Ts...>>::value)
                {
                    return std::tuple<Ts...>{std::forward<Ts>(ts)...};
                }
                template<typename F, typename S>
                std::pair<F, S> operator()(F && f, S && s) const
                    noexcept(std::is_nothrow_move_constructible<std::pair<F, S>>::value)
                {
                    return {std::forward<F>(f), std::forward<S>(s)};
                }
            };

            struct move_tuple_like_fn
            {
            private:
                template<typename T>
                using rvalue_ref_t =
                    meta::if_<
                        std::is_reference<T>,
                        meta::eval<std::remove_reference<T>> &&,
                        T>;

                template<typename ...Ts, std::size_t...Is>
                static std::tuple<rvalue_ref_t<Ts>...>
                impl(std::tuple<Ts...> &t, index_sequence<Is...>)
                {
                    return std::tuple<rvalue_ref_t<Ts>...>{std::move(std::get<Is>(t))...};
                }
                template<typename...Ts>
                using nothrow_move_ctors_t =
                    meta::and_<meta::or_<std::is_reference<rvalue_ref_t<Ts>>,
                        std::is_nothrow_move_constructible<rvalue_ref_t<Ts>>>...>;
            public:
                template<typename ...Ts>
                std::tuple<rvalue_ref_t<Ts>...> operator()(std::tuple<Ts...> && t) const
                    noexcept(std::is_nothrow_move_constructible<
                        std::tuple<rvalue_ref_t<Ts>...>>::value)
                {
                    return move_tuple_like_fn::impl(t, make_index_sequence<sizeof...(Ts)>{});
                }
                template<typename F, typename S>
                std::pair<rvalue_ref_t<F>, rvalue_ref_t<S>> operator()(std::pair<F, S> && p) const
                    noexcept(std::is_nothrow_move_constructible<
                        std::pair<rvalue_ref_t<F>, rvalue_ref_t<S>>>::value)
                {
                    return {std::move(p.first), std::move(p.second)};
                }
            };

            struct copy_tuple_like_fn
            {
                template<typename ...Ts>
                std::tuple<decay_t<Ts>...> operator()(std::tuple<Ts...> &&) const;
                template<typename F, typename S>
                std::pair<decay_t<F>, decay_t<S>> operator()(std::pair<F, S> &&) const;
            };

            template<typename Cur, typename Sent, typename Reference, typename MoveFun>
            struct zip_with_mixin : basic_mixin<Cur>
            {
                using basic_mixin<Cur>::basic_mixin;
                using I0 = basic_iterator<Cur, Sent>;
                using I1 = basic_iterator<Cur, Cur>;
                using R = Reference;
                friend auto indirect_move(I0 const &, R && t)
                    noexcept(noexcept(MoveFun{}(std::forward<R>(t)))) ->
                    decltype(MoveFun{}(std::forward<R>(t)))
                {
                    return MoveFun{}(std::forward<R>(t));
                }
                friend auto indirect_move(I1 const &, R && t)
                    noexcept(noexcept(MoveFun{}(std::forward<R>(t)))) ->
                    decltype(MoveFun{}(std::forward<R>(t)))
                {
                    return MoveFun{}(std::forward<R>(t));
                }
            };

            template<typename ...Ts>
            struct tuple_ref
              : std::tuple<Ts...>
            {
            private:
                template<std::size_t...Is>
                tuple_ref(std::tuple<decay_t<Ts>...> & val, index_sequence<Is...>)
                  : std::tuple<Ts...>{std::get<Is>(val)...}
                {}
            public:
                CONCEPT_REQUIRES(meta::and_c<(bool) DefaultConstructible<Ts>()...>::value)
                tuple_ref()
                  : std::tuple<Ts...>{}
                {}
                template<typename...Us,
                    CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us const &>()...>::value)>
                tuple_ref(std::tuple<Us...> const &that)
                  : std::tuple<Ts...>(that)
                {}
                template<typename...Us,
                    CONCEPT_REQUIRES_(meta::and_c<(bool) Constructible<Ts, Us &&>()...>::value)>
                tuple_ref(std::tuple<Us...> &&that)
                  : std::tuple<Ts...>(std::move(that))
                {}
                tuple_ref(std::tuple<decay_t<Ts>...> & val)
                  : tuple_ref{val, make_index_sequence<sizeof...(Ts)>{}}
                {}
                using std::tuple<Ts...>::operator=;
		CONCEPT_REQUIRES(meta::and_c<(bool) Constructible<decay_t<Ts>, Ts const &>()...>::value)
		operator std::tuple<decay_t<Ts>...> () const
		{
		    return static_cast<std::tuple<Ts...> const &>(*this);
		}
                CONCEPT_REQUIRES(meta::and_c<(bool) EqualityComparable<Ts>()...>::value)
                friend bool operator==(tuple_ref const &a, tuple_ref const &b)
                {
                    return a.base() == b.base();
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) EqualityComparable<Ts>()...>::value)
                friend bool operator!=(tuple_ref const &a, tuple_ref const &b)
                {
                    return a.base() != b.base();
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) TotallyOrdered<Ts>()...>::value)
                friend bool operator<(tuple_ref const &a, tuple_ref const &b)
                {
                    return a.base() < b.base();
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) TotallyOrdered<Ts>()...>::value)
                friend bool operator>(tuple_ref const &a, tuple_ref const &b)
                {
                    return a.base() > b.base();
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) TotallyOrdered<Ts>()...>::value)
                friend bool operator<=(tuple_ref const &a, tuple_ref const &b)
                {
                    return a.base() <= b.base();
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) TotallyOrdered<Ts>()...>::value)
                friend bool operator>=(tuple_ref const &a, tuple_ref const &b)
                {
                    return a.base() >= b.base();
                }
            };

            template<typename F, typename S>
            struct pair_ref
              : std::pair<F, S>
            {
            private:
                std::pair<F, S> &base() { return *this; }
                std::pair<F, S> const &base() const { return *this; }
            public:
                CONCEPT_REQUIRES(DefaultConstructible<F>() && DefaultConstructible<S>())
                pair_ref()
                  : std::pair<F, S>{}
                {}
                template<typename T, typename U,
                    CONCEPT_REQUIRES_(Constructible<F, T const &>() && Constructible<S, U const &>())>
                pair_ref(std::pair<T, U> const &that)
                  : std::pair<F, S>(that)
                {}
                template<typename T, typename U,
                    CONCEPT_REQUIRES_(Constructible<F, T &&>() && Constructible<S, U &&>())>
                pair_ref(std::pair<T, U> &&that)
                  : std::pair<F, S>(std::move(that))
                {}
                pair_ref(std::pair<decay_t<F>, decay_t<S>> & val)
                  : std::pair<F, S>{val.first, val.second}
                {}
                using std::pair<F, S>::operator=;
		CONCEPT_REQUIRES(Constructible<decay_t<F>, F const &>() &&
		    Constructible<decay_t<S>, S const &>())
		operator std::pair<decay_t<F>, decay_t<S>> () const
		{
		    return static_cast<std::pair<F, S> const &>(*this);
		}
                CONCEPT_REQUIRES(EqualityComparable<F>() && EqualityComparable<S>())
                friend bool operator==(pair_ref const &a, pair_ref const &b)
                {
                    return a.base() == b.base();
                }
                CONCEPT_REQUIRES(EqualityComparable<F>() && EqualityComparable<S>())
                friend bool operator!=(pair_ref const &a, pair_ref const &b)
                {
                    return a.base() != b.base();
                }
                CONCEPT_REQUIRES(TotallyOrdered<F>() && TotallyOrdered<S>())
                friend bool operator<(pair_ref const &a, pair_ref const &b)
                {
                    return a.base() < b.base();
                }
                CONCEPT_REQUIRES(TotallyOrdered<F>() && TotallyOrdered<S>())
                friend bool operator>(pair_ref const &a, pair_ref const &b)
                {
                    return a.base() > b.base();
                }
                CONCEPT_REQUIRES(TotallyOrdered<F>() && TotallyOrdered<S>())
                friend bool operator<=(pair_ref const &a, pair_ref const &b)
                {
                    return a.base() <= b.base();
                }
                CONCEPT_REQUIRES(TotallyOrdered<F>() && TotallyOrdered<S>())
                friend bool operator>=(pair_ref const &a, pair_ref const &b)
                {
                    return a.base() >= b.base();
                }
            };

            template<typename...Refs, typename...ValRefs>
            struct common_tuple_ref<std::tuple<Refs...> const &, std::tuple<ValRefs...> &>
            {
                using type =
                    tuple_ref<
                        common_reference_t<
                            remove_rvalue_reference_t<Refs> const &,
                            ValRefs &>...>;
            };

            template<typename F1, typename S1, typename F2, typename S2>
            struct common_tuple_ref<std::pair<F1, S1> const &, std::pair<F2, S2> &>
            {
                using type =
                    pair_ref<
                        common_reference_t<remove_rvalue_reference_t<F1> const &, F2 &>,
                        common_reference_t<remove_rvalue_reference_t<S1> const &, S2 &>>;
            };

        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Fun, typename...Rngs, typename CopyFun, typename MoveFun, typename CommonRef>
        struct zip_with_view<Fun, meta::list<Rngs...>, CopyFun, MoveFun, CommonRef>
          : range_facade<
                zip_with_view<Fun, meta::list<Rngs...>, CopyFun, MoveFun, CommonRef>,
                meta::and_c<is_infinite<Rngs>::value...>::value>
        {
        private:
            friend range_access;
            semiregular_invokable_t<Fun> fun_;
            std::tuple<view::all_t<Rngs>...> rngs_;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;
            using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;

            struct sentinel;
            struct cursor
            {
            private:
                friend struct sentinel;
                semiregular_invokable_ref_t<Fun, true> fun_;
                std::tuple<range_iterator_t<Rngs>...> its_;

                using reference_t_ =
                    concepts::Invokable::result_t<Fun, range_reference_t<Rngs> &&...>;
                using rvalue_reference_t_ =
                    concepts::Invokable::result_t<MoveFun, reference_t_ &&>;
            public:
                using difference_type = common_type_t<range_difference_t<Rngs>...>;
                using single_pass =
                    meta::or_c<(bool) Derived<ranges::input_iterator_tag,
                        range_category_t<Rngs>>()...>;
                using value_type =
                    detail::decay_t<concepts::Invokable::result_t<CopyFun, reference_t_ &&>>;
                using common_reference =
                    meta::apply<CommonRef,
                        detail::remove_rvalue_reference_t<reference_t_> const &,
                        value_type &>;
                // This is what gives zip_view iterators their special iter_move behavior:
                using mixin =
                    detail::zip_with_mixin<cursor, sentinel, reference_t_, MoveFun>;
                cursor() = default;
                cursor(semiregular_invokable_ref_t<Fun, true> fun,
                    std::tuple<range_iterator_t<Rngs>...> its)
                  : fun_(std::move(fun)), its_(std::move(its))
                {}
                reference_t_ current() const
                    noexcept(noexcept(fun_(std::declval<range_reference_t<Rngs>>()...)))
                {
                    return tuple_apply(fun_, tuple_transform(its_, detail::deref));
                }
                void next()
                {
                    tuple_for_each(its_, detail::inc);
                }
                bool equal(cursor const &that) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(its_, that.its_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalIterable<Rngs>()...>::value)
                void prev()
                {
                    tuple_for_each(its_, detail::dec);
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                void advance(difference_type n)
                {
                    using std::placeholders::_1;
                    tuple_for_each(its_, std::bind(detail::advance_, _1, n));
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                difference_type distance_to(cursor const &that) const
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accommodate zippers of sequences of different length.
                    if(0 < std::get<0>(that.its_) - std::get<0>(its_))
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::max)(),
                            detail::min_);
                    else
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::min)(),
                            detail::max_);
                }
            };

            struct sentinel
            {
            private:
                std::tuple<range_sentinel_t<Rngs>...> ends_;
            public:
                sentinel() = default;
                sentinel(
                    semiregular_invokable_ref_t<Fun, true> const &,
                    std::tuple<range_sentinel_t<Rngs>...> ends)
                  : ends_(std::move(ends))
                {}
                bool equal(cursor const &pos) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(pos.its_, ends_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
            };

            using are_bounded_t = meta::and_c<(bool) BoundedIterable<Rngs>()...>;

            cursor begin_cursor()
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            meta::if_<are_bounded_t, cursor, sentinel> end_cursor()
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Iterable<Rngs const>()...>::value)
            cursor begin_cursor() const
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Iterable<Rngs const>()...>::value)
            meta::if_<are_bounded_t, cursor, sentinel> end_cursor() const
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
        public:
            zip_with_view() = default;
            explicit zip_with_view(Rngs &&...rngs)
              : fun_(invokable(Fun{}))
              , rngs_{view::all(std::forward<Rngs>(rngs))...}
            {}
            explicit zip_with_view(Fun fun, Rngs &&...rngs)
              : fun_(invokable(std::move(fun)))
              , rngs_{view::all(std::forward<Rngs>(rngs))...}
            {}
            CONCEPT_REQUIRES(meta::and_c<(bool) SizedIterable<Rngs>()...>::value)
            size_type_ size() const
            {
                return tuple_foldl(
                    tuple_transform(rngs_, ranges::size),
                    (std::numeric_limits<size_type_>::max)(),
                    detail::min_);
            }
        };

        namespace view
        {
            struct zip_fn
            {
                template<typename ...Rngs>
                using Concept = meta::and_<InputIterable<Rngs>...>;

                template<typename...Rngs, CONCEPT_REQUIRES_(Concept<Rngs...>())>
                zip_view<meta::list<Rngs...>> operator()(Rngs &&... rngs) const
                {
                    CONCEPT_ASSERT(meta::and_c<(bool) Iterable<Rngs>()...>::value);
                    return zip_view<meta::list<Rngs...>>{std::forward<Rngs>(rngs)...};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename...Rngs, CONCEPT_REQUIRES_(!Concept<Rngs...>())>
                void operator()(Rngs &&... rngs) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputIterable<Rngs>...>(),
                        "All of the objects passed to view::zip must model the InputIterable "
                        "concept");
                }
            #endif
            };

            /// \relates zip_fn
            /// \ingroup group-views
            constexpr zip_fn zip{};

            struct zip_with_fn
            {
                template<typename Fun, typename ...Rngs>
                using Concept = meta::and_<
                    InputIterable<Rngs>...,
                    Invokable<Fun, range_common_reference_t<Rngs>...>>;

                template<typename Fun, typename...Rngs, CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
                zip_with_view<Fun, meta::list<Rngs...>> operator()(Fun fun, Rngs &&... rngs) const
                {
                    return zip_with_view<Fun, meta::list<Rngs...>>{
                        std::move(fun),
                        std::forward<Rngs>(rngs)...
                    };
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Fun, typename...Rngs, CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
                void operator()(Fun, Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputIterable<Rngs>...>(),
                        "All of the objects passed to view::zip_with must model the InputIterable "
                        "concept");
                    CONCEPT_ASSERT_MSG(Invokable<Fun, range_common_reference_t<Rngs>...>(),
                        "The function passed to view::zip_with must be callable with N arguments "
                        "taken one from each of the N ranges.");
                }
            #endif
            };

            /// \relates zip_with_fn
            /// \ingroup group-views
            constexpr zip_with_fn zip_with{};
        }
        /// @}
    }
}

#endif
