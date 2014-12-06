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
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng, typename Int>
            range_iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::InputIterable *,
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
            struct slice_view_
              : range_facade<slice_view<Rng>, false>
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
                slice_view_() = default;
                slice_view_(Rng && rng, difference_type_ from, difference_type_ count)
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
            struct slice_view_<Rng, true>
              : range_interface<slice_view<Rng>>
            {
            private:
                using base_range_t = view::all_t<Rng>;
                using difference_type_ = range_difference_t<Rng>;
                base_range_t rng_;
                difference_type_ from_, count_;
            public:
                slice_view_() = default;
                slice_view_(Rng && rng, difference_type_ from, difference_type_ count)
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
        /// \endcond

        /// \cond
        namespace adl_begin_end_detail
        {
            template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
            detail::from_end_<meta::eval<std::make_signed<Int>>> operator-(end_fn, Int dist)
            {
                RANGES_ASSERT(0 <= static_cast<meta::eval<std::make_signed<Int>>>(dist));
                return {-static_cast<meta::eval<std::make_signed<Int>>>(dist)};
            }
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct slice_view
          : detail::slice_view_<Rng>
        {
            using detail::slice_view_<Rng>::slice_view_;
        };

        namespace view
        {
            struct slice_fn
            {
            private:
                friend view_access;

                template<typename Rng>
                static slice_view<Rng>
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

                // Overloads for the pipe syntax: rng | view::slice(from,to)
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(slice_fn slice, Int from, Int to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(slice, std::placeholders::_1, from, to))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(slice_fn slice, Int from, detail::from_end_<Int> to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(slice, std::placeholders::_1, from, to))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(slice_fn slice, detail::from_end_<Int> from, detail::from_end_<Int> to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(slice, std::placeholders::_1, from, to))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(slice_fn slice, Int from, end_fn)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(ranges::view::drop, std::placeholders::_1, from))
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(slice_fn slice, detail::from_end_<Int> from, end_fn to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(slice, std::placeholders::_1, from, to))
                )

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
                        distance(rng) + to.dist_ - from, iterable_concept<Rng>{}))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_ASSERT(0 <= from);
                    RANGES_ASSERT(from <= distance(rng) + to.dist_);
                    return slice_fn::invoke_(std::forward<Rng>(rng), from,
                        distance(rng) + to.dist_ - from, iterable_concept<Rng>{});
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
                    decltype(ranges::view::drop(std::forward<Rng>(rng), from))
                {
                    RANGES_ASSERT(0 <= from);
                    return ranges::view::drop(std::forward<Rng>(rng), from);
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

            #ifndef RANGES_DOXYGEN_INVOKED
                //
                // These overloads are strictly so that users get better error messages
                // when they try to slice things in a way that doesn't support the operation.
                //

                // slice(rng, 2, 4)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!InputIterable<Rng>())>
                void operator()(Rng &&, range_difference_t<Rng>, range_difference_t<Rng>) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object to be sliced must be a model of the InputIterable concept.");
                }
                // slice(rng, 4, end-2)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!(InputIterable<Rng>() && SizedIterable<Rng>()))>
                void operator()(Rng &&, range_difference_t<Rng>,
                    detail::from_end_<range_difference_t<Rng>>) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object to be sliced must be a model of the InputIterable concept.");
                    CONCEPT_ASSERT_MSG(SizedIterable<Rng>(),
                        "When slicing a range with a positive start offset and a stop offset "
                        "measured from the end, the range must be a model of the SizedIterable "
                        "concept; that is, its size must be known.");
                }
                // slice(rng, end-4, end-2)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!((InputIterable<Rng>() && SizedIterable<Rng>()) ||
                        ForwardIterable<Rng>()))>
                void operator()(Rng &&, detail::from_end_<range_difference_t<Rng>>,
                    detail::from_end_<range_difference_t<Rng>>) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object to be sliced must be a model of the InputIterable concept.");
                    CONCEPT_ASSERT_MSG(SizedIterable<Rng>() || ForwardIterable<Rng>(),
                        "When slicing a range with a start and stop offset measured from the end, "
                        "the range must either be a model of the SizedIterable concept (its size "
                        "must be known), or it must be a model of the ForwardIterable concept.");
                }
                // slice(rng, 4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!(InputIterable<Rng>()))>
                void operator()(Rng &&, range_difference_t<Rng>, end_fn) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object to be sliced must be a model of the InputIterable concept.");
                }
                // slice(rng, end-4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!((InputIterable<Rng>() && SizedIterable<Rng>()) ||
                        ForwardIterable<Rng>()))>
                void operator()(Rng &&, detail::from_end_<range_difference_t<Rng>>, end_fn) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object to be sliced must be a model of the InputIterable concept.");
                    CONCEPT_ASSERT_MSG(SizedIterable<Rng>() || ForwardIterable<Rng>(),
                        "When slicing a range with a start and stop offset measured from the end, "
                        "the range must either be a model of the SizedIterable concept (its size "
                        "must be known), or it must be a model of the ForwardIterable concept.");
                }
            #endif
            };

            /// \sa `slice_fn`
            /// \ingroup group-views
            constexpr view<slice_fn> slice{};
        }
        /// @}
    }
}

#endif
