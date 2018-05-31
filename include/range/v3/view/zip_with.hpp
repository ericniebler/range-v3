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

#ifndef RANGES_V3_VIEW_ZIP_WITH_HPP
#define RANGES_V3_VIEW_ZIP_WITH_HPP

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            struct equal_to_
            {
                template<typename T, typename U>
                bool operator()(T const &t, U const &u) const
                {
                    return static_cast<bool>(t == u);
                }
            };
            RANGES_INLINE_VARIABLE(equal_to_, equal_to)

            struct dec_
            {
                template<typename T>
                void operator()(T & t) const
                {
                    --t;
                }
            };
            RANGES_INLINE_VARIABLE(dec_, dec)

            struct inc_
            {
                template<typename T>
                void operator()(T & t) const
                {
                    ++t;
                }
            };
            RANGES_INLINE_VARIABLE(inc_, inc)

            struct _advance_
            {
                template<typename I, CONCEPT_REQUIRES_(Iterator<I>())>
                void operator()(I & i, difference_type_t<I> n) const
                {
                    advance(i, n);
                }
            };
            RANGES_INLINE_VARIABLE(_advance_, advance_)

            struct distance_to_
            {
                template<typename T>
                constexpr auto operator()(T const &t, T const &u) const ->
                    decltype(u - t)
                {
                    return u - t;
                }
            };
            RANGES_INLINE_VARIABLE(distance_to_, distance_to)

            struct _min_
            {
                template<typename T, typename U>
                constexpr auto operator()(T const &t, U const &u) const ->
                    decltype(true ? t : u)
                {
                    return t < u ? t : u;
                }
            };
            RANGES_INLINE_VARIABLE(_min_, min_)

            struct _max_
            {
                template<typename T, typename U>
                constexpr auto operator()(T const &t, U const &u) const ->
                    decltype(true ? u : t)
                {
                    return t < u ? u : t;
                }
            };
            RANGES_INLINE_VARIABLE(_max_, max_)

            template<typename State, typename Value>
            using zip_cardinality =
                std::integral_constant<cardinality,
                    State::value >= 0 || Value::value >= 0 ?
                        (State::value >= 0 && Value::value >= 0 ? min_(State::value, Value::value) : finite) :
                        State::value == finite || Value::value == finite ?
                            finite :
                            State::value == unknown || Value::value == unknown ?
                                unknown :
                                infinite>;
        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Fun, typename...Rngs>
        struct iter_zip_with_view
          : view_facade<
                iter_zip_with_view<Fun, Rngs...>,
                meta::fold<
                    meta::list<range_cardinality<Rngs>...>,
                    std::integral_constant<cardinality, infinite>,
                    meta::quote<detail::zip_cardinality>>::value>
        {
        private:
            friend range_access;
            semiregular_t<Fun> fun_;
            std::tuple<Rngs...> rngs_;
            using difference_type_ = common_type_t<range_difference_type_t<Rngs>...>;
            using size_type_ = meta::_t<std::make_unsigned<difference_type_>>;

            struct cursor;

            struct sentinel
            {
            private:
                friend struct cursor;
                std::tuple<sentinel_t<Rngs>...> ends_;
            public:
                sentinel() = default;
                sentinel(detail::any, std::tuple<sentinel_t<Rngs>...> ends)
                  : ends_(std::move(ends))
                {}
            };

            struct cursor
            {
            private:
                using fun_ref_ = semiregular_ref_or_val_t<Fun, true>;
                fun_ref_ fun_;
                std::tuple<iterator_t<Rngs>...> its_;

            public:
                using difference_type =
                    common_type_t<range_difference_type_t<Rngs>...>;
                using single_pass =
                    meta::or_c<(bool) SinglePass<iterator_t<Rngs>>()...>;
                using value_type =
                    detail::decay_t<decltype(invoke(fun_, copy_tag{}, iterator_t<Rngs>{}...))>;

                cursor() = default;
                cursor(fun_ref_ fun, std::tuple<iterator_t<Rngs>...> its)
                  : fun_(std::move(fun)), its_(std::move(its))
                {}
                auto read() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    tuple_apply(fun_, its_)
                )
                void next()
                {
                    tuple_for_each(its_, detail::inc);
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) Sentinel<iterator_t<Rngs>, iterator_t<Rngs>>()...>::value)
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
                bool equal(sentinel const &s) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(its_, s.ends_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalRange<Rngs>()...>::value)
                void prev()
                {
                    tuple_for_each(its_, detail::dec);
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessRange<Rngs>()...>::value)
                void advance(difference_type n)
                {
                    using std::placeholders::_1;
                    tuple_for_each(its_, std::bind(detail::advance_, _1, n));
                }
                CONCEPT_REQUIRES(meta::and_c<(bool)
                    SizedSentinel<iterator_t<Rngs>, iterator_t<Rngs>>()...>::value)
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
                template<std::size_t...Is>
                auto move_(meta::index_sequence<Is...>) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    invoke(fun_, move_tag{}, std::get<Is>(its_)...)
                )
                auto move() const
                    noexcept(noexcept(std::declval<cursor const&>().move_(
                        meta::make_index_sequence<sizeof...(Rngs)>{}))) ->
                    decltype(std::declval<cursor const&>().move_(
                        meta::make_index_sequence<sizeof...(Rngs)>{}))
                {
                    return move_(meta::make_index_sequence<sizeof...(Rngs)>{});
                }
            };

            using end_cursor_t =
                meta::if_<
                    meta::and_c<
                        meta::and_c<(bool) BoundedRange<Rngs>()...>::value,
                        !SinglePass<iterator_t<Rngs>>()...>,
                    cursor,
                    sentinel>;

            cursor begin_cursor()
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            end_cursor_t end_cursor()
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Range<Rngs const>()...>::value)
            cursor begin_cursor() const
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Range<Rngs const>()...>::value)
            end_cursor_t end_cursor() const
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
        public:
            iter_zip_with_view() = default;
            explicit iter_zip_with_view(Rngs ...rngs)
              : fun_(Fun{})
              , rngs_{std::move(rngs)...}
            {}
            explicit iter_zip_with_view(Fun fun, Rngs ...rngs)
              : fun_(std::move(fun))
              , rngs_{std::move(rngs)...}
            {}
            CONCEPT_REQUIRES(meta::and_c<(bool) SizedRange<Rngs>()...>::value)
            constexpr size_type_ size() const
            {
                return range_cardinality<iter_zip_with_view>::value >= 0 ?
                    (size_type_)range_cardinality<iter_zip_with_view>::value :
                    tuple_foldl(
                        tuple_transform(rngs_, ranges::size),
                        (std::numeric_limits<size_type_>::max)(),
                        detail::min_);
            }
        };

        template<typename Fun, typename...Rngs>
        struct zip_with_view
          : iter_zip_with_view<indirected<Fun>, Rngs...>
        {
            zip_with_view() = default;
            explicit zip_with_view(Rngs ...rngs)
              : iter_zip_with_view<indirected<Fun>, Rngs...>{
                  {Fun{}}, std::move(rngs)...}
            {}
            explicit zip_with_view(Fun fun, Rngs ...rngs)
              : iter_zip_with_view<indirected<Fun>, Rngs...>{
                  {std::move(fun)}, std::move(rngs)...}
            {}
        };

        namespace view
        {
            struct iter_zip_with_fn
            {
                template<typename Fun, typename ...Rngs>
                using Concept = meta::and_<
                    InputRange<Rngs>...,
                    CopyConstructible<Fun>,
                    Invocable<Fun&, iterator_t<Rngs>...>,
                    Invocable<Fun&, copy_tag, iterator_t<Rngs>...>,
                    Invocable<Fun&, move_tag, iterator_t<Rngs>...>>;

                template<typename...Rngs, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
                iter_zip_with_view<Fun, all_t<Rngs>...> operator()(Fun fun, Rngs &&... rngs) const
                {
                    return iter_zip_with_view<Fun, all_t<Rngs>...>{
                        std::move(fun),
                        all(static_cast<Rngs&&>(rngs))...
                    };
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Fun, typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
                void operator()(Fun, Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputRange<Rngs>...>(),
                        "All of the objects passed to view::iter_zip_with must model the InputRange "
                        "concept");
                    CONCEPT_ASSERT_MSG(
                        CopyConstructible<Fun>(),
                        "The function object passed to view::iter_zip_with must be CopyConstructible.");
                    CONCEPT_ASSERT_MSG(
                        Invocable<Fun&, iterator_t<Rngs>...>(),
                        "The function passed to view::iter_zip_with must be callable with arguments "
                        "of the ranges' iterator types.");
                    CONCEPT_ASSERT_MSG(
                        Invocable<Fun&, copy_tag, iterator_t<Rngs>...>(),
                        "The function passed to view::iter_zip_with must be callable with "
                        "copy_tag and arguments of the ranges' iterator types.");
                    CONCEPT_ASSERT_MSG(
                        Invocable<Fun&, move_tag, iterator_t<Rngs>...>(),
                        "The function passed to view::iter_zip_with must be callable with "
                        "move_tag and arguments of the ranges' iterator types.");
                }
            #endif
            };

            /// \relates iter_zip_with_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(iter_zip_with_fn, iter_zip_with)

            struct zip_with_fn
            {
                template<typename Fun, typename ...Rngs>
                using Concept = meta::and_<
                    meta::and_<InputRange<Rngs>...>,
                    CopyConstructible<Fun>,
                    Invocable<Fun&, range_reference_t<Rngs> &&...>>;

                template<typename...Rngs, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
                zip_with_view<Fun, all_t<Rngs>...> operator()(Fun fun, Rngs &&... rngs) const
                {
                    return zip_with_view<Fun, all_t<Rngs>...>{
                        std::move(fun),
                        all(static_cast<Rngs&&>(rngs))...
                    };
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Fun, typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
                void operator()(Fun, Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputRange<Rngs>...>(),
                        "All of the objects passed to view::zip_with must model the InputRange "
                        "concept");
                    CONCEPT_ASSERT_MSG(
                        CopyConstructible<Fun>(),
                        "The function object passed to view::zip_with must be CopyConstructible.");
                    CONCEPT_ASSERT_MSG(
                        Invocable<Fun&, range_reference_t<Rngs> &&...>(),
                        "The function passed to view::zip_with must be callable with arguments "
                        "of the ranges' reference types.");
                }
            #endif
            };

            /// \relates zip_with_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(zip_with_fn, zip_with)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::iter_zip_with_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::v3::zip_with_view)

#endif
