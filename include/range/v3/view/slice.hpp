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

#ifndef RANGES_V3_VIEW_SLICE_HPP
#define RANGES_V3_VIEW_SLICE_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/drop.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Rng, typename Int>
            range_iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::BidirectionalIterable *,
                std::true_type)
            {
                RANGES_ASSERT(0 <= i);
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, typename Int>
            range_iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::BidirectionalIterable *,
                std::false_type)
            {
                if(0 > i)
                {
                    // If it's not bounded and we know the size, faster to count from the front
                    if(SizedIterable<Rng>() && !BoundedIterable<Rng>())
                        return next(ranges::begin(rng), distance(rng) + i);
                    // Otherwise, probably faster to count from the back.
                    return next(next_to(ranges::begin(rng), ranges::end(rng)), i);
                }
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, typename Int>
            range_iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::InputIterable *,
                std::false_type)
            {
                RANGES_ASSERT(i >= 0 || SizedIterable<Rng>() || ForwardIterable<Rng>());
                if(0 > i)
                    return next(ranges::begin(rng), distance(rng) + i);
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, bool IsRandomAccess = RandomAccessIterable<Rng>()>
            struct sliced_view_
              : range_facade<sliced_view<Rng>, false>
            {
            private:
                friend range_access;
                using base_range_t = view::all_t<Rng>;
                using difference_type_ = range_difference_t<Rng>;
                base_range_t rng_;
                difference_type_ from_, count_;

                detail::counted_cursor<range_iterator_t<Rng>> begin_cursor()
                {
                    return {detail::pos_at_(rng_, from_, iterable_concept<Rng>{},
                        is_infinite<Rng>{}), count_};
                }
                CONCEPT_REQUIRES(Iterable<Rng const>())
                detail::counted_cursor<range_iterator_t<Rng const>> begin_cursor() const
                {
                    return {detail::pos_at_(rng_, from_, iterable_concept<Rng>{},
                        is_infinite<Rng>{}), count_};
                }
                detail::counted_sentinel end_cursor() const
                {
                    return {};
                }
            public:
                sliced_view_() = default;
                sliced_view_(Rng && rng, difference_type_ from, difference_type_ count)
                  : rng_(view::all(std::forward<Rng>(rng))), from_(from), count_(count)
                {}
                range_size_t<Rng> size() const
                {
                    return static_cast<range_size_t<Rng>>(count_);
                }
                base_range_t & base()
                {
                    return rng_;
                }
                base_range_t const & base() const
                {
                    return rng_;
                }
            };

            template<typename Rng>
            struct sliced_view_<Rng, true>
              : range_interface<sliced_view<Rng>>
            {
            private:
                using base_range_t = view::all_t<Rng>;
                using difference_type_ = range_difference_t<Rng>;
                base_range_t rng_;
                difference_type_ from_, count_;
            public:
                sliced_view_() = default;
                sliced_view_(Rng && rng, difference_type_ from, difference_type_ count)
                  : rng_(view::all(std::forward<Rng>(rng))), from_(from), count_(count)
                {
                    RANGES_ASSERT(0 <= count_);
                }
                range_iterator_t<Rng> begin()
                {
                    return detail::pos_at_(rng_, from_, iterable_concept<Rng>{},
                        is_infinite<Rng>{});
                }
                range_iterator_t<Rng> end()
                {
                    return detail::pos_at_(rng_, from_, iterable_concept<Rng>{},
                        is_infinite<Rng>{}) + count_;
                }
                CONCEPT_REQUIRES(Iterable<Rng const>())
                range_iterator_t<Rng const> begin() const
                {
                    return detail::pos_at_(rng_, from_, iterable_concept<Rng>{},
                        is_infinite<Rng>{});
                }
                CONCEPT_REQUIRES(Iterable<Rng const>())
                range_iterator_t<Rng const> end() const
                {
                    return detail::pos_at_(rng_, from_, iterable_concept<Rng>{},
                        is_infinite<Rng>{}) + count_;
                }
                range_size_t<Rng> size() const
                {
                    return static_cast<range_size_t<Rng>>(count_);
                }
                base_range_t & base()
                {
                    return rng_;
                }
                base_range_t const & base() const
                {
                    return rng_;
                }
            };
        }

        namespace adl_begin_end_detail
        {
            template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
            detail::from_end_<meta_eval<std::make_signed<Int>>> operator-(end_fn, Int dist)
            {
                RANGES_ASSERT(0 <= static_cast<meta_eval<std::make_signed<Int>>>(dist));
                return {-static_cast<meta_eval<std::make_signed<Int>>>(dist)};
            }
        }

        template<typename Rng>
        struct sliced_view
          : detail::sliced_view_<Rng>
        {
            using detail::sliced_view_<Rng>::sliced_view_;
        };

        namespace view
        {
            struct slice_fn
            {
            private:
                template<typename Rng>
                static sliced_view<Rng>
                invoke_(Rng && rng, range_difference_t<Rng> from, range_difference_t<Rng> count,
                    concepts::InputIterable *, concepts::Iterable * = nullptr)
                {
                    return {std::forward<Rng>(rng), from, count};
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Range<Rng>() && std::is_lvalue_reference<Rng>())>
                static range<range_iterator_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> from, range_difference_t<Rng> count,
                    concepts::RandomAccessIterable *, concepts::BoundedIterable * = nullptr)
                {
                    auto it = detail::pos_at_(rng, from, iterable_concept<Rng>{}, is_infinite<Rng>{});
                    return {it, it + count};
                }
            public:
                // slice(rng, 2, 4)
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputIterable<Rng>())>
                auto operator()(Rng && rng, range_difference_t<Rng> from,
                    range_difference_t<Rng> to) const ->
                    decltype(slice_fn::invoke_(std::forward<Rng>(rng), from, to - from,
                        iterable_concept<Rng>{}))
                {
                    RANGES_ASSERT(0 <= from && from <= to);
                    return slice_fn::invoke_(std::forward<Rng>(rng), from, to - from,
                        iterable_concept<Rng>{});
                }
                // slice(rng, 4, end-2)
                //  TODO Support Forward, non-Sized iterables by returning a range that
                //       doesn't know it's size?
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputIterable<Rng>() && SizedIterable<Rng>())>
                auto operator()(Rng && rng, range_difference_t<Rng> from,
                    detail::from_end_<range_difference_t<Rng>> to) const ->
                    decltype(slice_fn::invoke_(std::forward<Rng>(rng), from,
                        distance(rng) + to.dist_, iterable_concept<Rng>{}))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_ASSERT(0 <= from);
                    RANGES_ASSERT(from <= distance(rng) + to.dist_);
                    return slice_fn::invoke_(std::forward<Rng>(rng), from,
                        distance(rng) + to.dist_, iterable_concept<Rng>{});
                }
                // slice(rng, end-4, end-2)
                template<typename Rng,
                    CONCEPT_REQUIRES_((InputIterable<Rng>() && SizedIterable<Rng>()) ||
                        ForwardIterable<Rng>())>
                auto operator()(Rng && rng, detail::from_end_<range_difference_t<Rng>> from,
                    detail::from_end_<range_difference_t<Rng>> to) const ->
                    decltype(slice_fn::invoke_(std::forward<Rng>(rng), from.dist_,
                        to.dist_ - from.dist_, iterable_concept<Rng>{},
                        bounded_iterable_concept<Rng>{}()))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_ASSERT(from.dist_ <= to.dist_);
                    return slice_fn::invoke_(std::forward<Rng>(rng), from.dist_,
                        to.dist_ - from.dist_, iterable_concept<Rng>{},
                        bounded_iterable_concept<Rng>{}());
                }
                // slice(rng, 4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputIterable<Rng>())>
                auto operator()(Rng && rng, range_difference_t<Rng> from, end_fn) const ->
                    decltype(view::drop(std::forward<Rng>(rng), from))
                {
                    RANGES_ASSERT(0 <= from);
                    return view::drop(std::forward<Rng>(rng), from);
                }
                // slice(rng, end-4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_((InputIterable<Rng>() && SizedIterable<Rng>()) ||
                        ForwardIterable<Rng>())>
                auto operator()(Rng && rng, detail::from_end_<range_difference_t<Rng>> from,
                    end_fn) const ->
                    decltype(slice_fn::invoke_(std::forward<Rng>(rng), from.dist_,
                        -from.dist_, iterable_concept<Rng>{},
                        bounded_iterable_concept<Rng>{}()))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    return slice_fn::invoke_(std::forward<Rng>(rng), from.dist_,
                        -from.dist_, iterable_concept<Rng>{},
                        bounded_iterable_concept<Rng>{}());
                }

                // Overloads for the pipe syntax: rng | view::slice(from,to)
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                auto operator()(Int from, Int to) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(*this, std::placeholders::_1, from, to))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                auto operator()(Int from, detail::from_end_<Int> to) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(*this, std::placeholders::_1, from, to))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                auto operator()(detail::from_end_<Int> from, detail::from_end_<Int> to) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(*this, std::placeholders::_1, from, to))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                auto operator()(Int from, end_fn) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(view::drop, std::placeholders::_1, from))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                auto operator()(detail::from_end_<Int> from, end_fn to) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(*this, std::placeholders::_1, from, to))
                )
            };

            RANGES_CONSTEXPR slice_fn slice {};
        }
    }
}

#endif
