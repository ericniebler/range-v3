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

#ifndef RANGES_V3_VIEW_SLICE_HPP
#define RANGES_V3_VIEW_SLICE_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop_exactly.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng, typename Int>
            iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::InputRange *,
                std::true_type)
            {
                RANGES_EXPECT(0 <= i);
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, typename Int>
            iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::BidirectionalRange *,
                std::false_type)
            {
                if(0 > i)
                {
                    // If it's not bounded and we know the size, faster to count from the front
                    if(SizedRange<Rng>() && !BoundedRange<Rng>())
                        return next(ranges::begin(rng), distance(rng) + i);
                    // Otherwise, probably faster to count from the back.
                    return next(ranges::next(ranges::begin(rng), ranges::end(rng)), i);
                }
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, typename Int>
            iterator_t<Rng> pos_at_(Rng && rng, Int i, concepts::InputRange *,
                std::false_type)
            {
                RANGES_EXPECT(i >= 0 || SizedRange<Rng>() || ForwardRange<Rng>());
                if(0 > i)
                    return next(ranges::begin(rng), distance(rng) + i);
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, bool IsRandomAccess = RandomAccessRange<Rng>()>
            struct slice_view_
              : view_facade<slice_view<Rng>, finite>
            {
            private:
                friend range_access;
                using difference_type_ = range_difference_type_t<Rng>;
                Rng rng_;
                difference_type_ from_, count_;
                detail::non_propagating_cache<iterator_t<Rng>> begin_;

                iterator_t<Rng> get_begin_()
                {
                    if(!begin_)
                        begin_ = detail::pos_at_(rng_, from_, range_concept<Rng>{},
                            is_infinite<Rng>{});
                    return *begin_;
                }
            public:
                slice_view_() = default;
                slice_view_(Rng rng, difference_type_ from, difference_type_ count)
                  : rng_(std::move(rng)), from_(from), count_(count)
                {}
                counted_iterator<iterator_t<Rng>> begin()
                {
                    return make_counted_iterator(get_begin_(), count_);
                }
                default_sentinel end()
                {
                    return {};
                }
                range_size_type_t<Rng> size() const
                {
                    return static_cast<range_size_type_t<Rng>>(count_);
                }
                Rng & base()
                {
                    return rng_;
                }
                Rng const & base() const
                {
                    return rng_;
                }
            };

            template<typename Rng>
            struct slice_view_<Rng, true>
              : view_interface<slice_view<Rng>, finite>
            {
            private:
                using difference_type_ = range_difference_type_t<Rng>;
                Rng rng_;
                difference_type_ from_, count_;
            public:
                slice_view_() = default;
                slice_view_(Rng rng, difference_type_ from, difference_type_ count)
                  : rng_(std::move(rng)), from_(from), count_(count)
                {
                    RANGES_EXPECT(0 <= count_);
                }
                iterator_t<Rng> begin()
                {
                    return detail::pos_at_(rng_, from_, range_concept<Rng>{},
                        is_infinite<Rng>{});
                }
                iterator_t<Rng> end()
                {
                    return detail::pos_at_(rng_, from_, range_concept<Rng>{},
                        is_infinite<Rng>{}) + count_;
                }
                template<typename BaseRng = Rng,
                    CONCEPT_REQUIRES_(Range<BaseRng const>())>
                iterator_t<BaseRng const> begin() const
                {
                    return detail::pos_at_(rng_, from_, range_concept<Rng>{},
                        is_infinite<Rng>{});
                }
                template<typename BaseRng = Rng,
                    CONCEPT_REQUIRES_(Range<BaseRng const>())>
                iterator_t<BaseRng const> end() const
                {
                    return detail::pos_at_(rng_, from_, range_concept<Rng>{},
                        is_infinite<Rng>{}) + count_;
                }
                range_size_type_t<Rng> size() const
                {
                    return static_cast<range_size_type_t<Rng>>(count_);
                }
                Rng & base()
                {
                    return rng_;
                }
                Rng const & base() const
                {
                    return rng_;
                }
            };
        }
        /// \endcond

        /// \cond
        namespace _end_
        {
            template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
            detail::from_end_<meta::_t<std::make_signed<Int>>> operator-(end_fn, Int dist)
            {
                RANGES_EXPECT(0 <= static_cast<meta::_t<std::make_signed<Int>>>(dist));
                return {-static_cast<meta::_t<std::make_signed<Int>>>(dist)};
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
                static slice_view<all_t<Rng>>
                invoke_(Rng && rng, range_difference_type_t<Rng> from, range_difference_type_t<Rng> count,
                    concepts::InputRange *, concepts::Range * = nullptr)
                {
                    return {all(static_cast<Rng&&>(rng)), from, count};
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(!View<uncvref_t<Rng>>() && std::is_lvalue_reference<Rng>())>
                static iterator_range<iterator_t<Rng>>
                invoke_(Rng && rng, range_difference_type_t<Rng> from, range_difference_type_t<Rng> count,
                    concepts::RandomAccessRange *, concepts::BoundedRange * = nullptr)
                {
                    auto it = detail::pos_at_(rng, from, range_concept<Rng>{}, is_infinite<Rng>{});
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
                static auto bind(slice_fn, Int from, end_fn)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(ranges::view::drop_exactly, std::placeholders::_1, from))
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
                    CONCEPT_REQUIRES_(InputRange<Rng>())>
                auto operator()(Rng && rng, range_difference_type_t<Rng> from,
                    range_difference_type_t<Rng> to) const ->
                    decltype(slice_fn::invoke_(static_cast<Rng&&>(rng), from, to - from,
                        range_concept<Rng>{}))
                {
                    RANGES_EXPECT(0 <= from && from <= to);
                    return slice_fn::invoke_(static_cast<Rng&&>(rng), from, to - from,
                        range_concept<Rng>{});
                }
                // slice(rng, 4, end-2)
                //  TODO Support Forward, non-Sized ranges by returning a range that
                //       doesn't know it's size?
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputRange<Rng>() && SizedRange<Rng>())>
                auto operator()(Rng && rng, range_difference_type_t<Rng> from,
                    detail::from_end_<range_difference_type_t<Rng>> to) const ->
                    decltype(slice_fn::invoke_(static_cast<Rng&&>(rng), from,
                        distance(rng) + to.dist_ - from, range_concept<Rng>{}))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_EXPECT(0 <= from);
                    RANGES_ASSERT(from <= distance(rng) + to.dist_);
                    return slice_fn::invoke_(static_cast<Rng&&>(rng), from,
                        distance(rng) + to.dist_ - from, range_concept<Rng>{});
                }
                // slice(rng, end-4, end-2)
                template<typename Rng,
                    CONCEPT_REQUIRES_((InputRange<Rng>() && SizedRange<Rng>()) ||
                        ForwardRange<Rng>())>
                auto operator()(Rng && rng, detail::from_end_<range_difference_type_t<Rng>> from,
                    detail::from_end_<range_difference_type_t<Rng>> to) const ->
                    decltype(slice_fn::invoke_(static_cast<Rng&&>(rng), from.dist_,
                        to.dist_ - from.dist_, range_concept<Rng>{},
                        bounded_range_concept<Rng>{}()))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_EXPECT(from.dist_ <= to.dist_);
                    return slice_fn::invoke_(static_cast<Rng&&>(rng), from.dist_,
                        to.dist_ - from.dist_, range_concept<Rng>{},
                        bounded_range_concept<Rng>{}());
                }
                // slice(rng, 4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputRange<Rng>())>
                auto operator()(Rng && rng, range_difference_type_t<Rng> from, end_fn) const ->
                    decltype(ranges::view::drop_exactly(static_cast<Rng&&>(rng), from))
                {
                    RANGES_EXPECT(0 <= from);
                    return ranges::view::drop_exactly(static_cast<Rng&&>(rng), from);
                }
                // slice(rng, end-4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_((InputRange<Rng>() && SizedRange<Rng>()) ||
                        ForwardRange<Rng>())>
                auto operator()(Rng && rng, detail::from_end_<range_difference_type_t<Rng>> from,
                    end_fn) const ->
                    decltype(slice_fn::invoke_(static_cast<Rng&&>(rng), from.dist_,
                        -from.dist_, range_concept<Rng>{},
                        bounded_range_concept<Rng>{}()))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    return slice_fn::invoke_(static_cast<Rng&&>(rng), from.dist_,
                        -from.dist_, range_concept<Rng>{},
                        bounded_range_concept<Rng>{}());
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                //
                // These overloads are strictly so that users get better error messages
                // when they try to slice things in a way that doesn't support the operation.
                //

                // slice(rng, 2, 4)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!InputRange<Rng>())>
                void operator()(Rng &&, range_difference_type_t<Rng>, range_difference_type_t<Rng>) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be sliced must be a model of the InputRange concept.");
                }
                // slice(rng, 4, end-2)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!(InputRange<Rng>() && SizedRange<Rng>()))>
                void operator()(Rng &&, range_difference_type_t<Rng>,
                    detail::from_end_<range_difference_type_t<Rng>>) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be sliced must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(SizedRange<Rng>(),
                        "When slicing a range with a positive start offset and a stop offset "
                        "measured from the end, the range must be a model of the SizedRange "
                        "concept; that is, its size must be known.");
                }
                // slice(rng, end-4, end-2)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!((InputRange<Rng>() && SizedRange<Rng>()) ||
                        ForwardRange<Rng>()))>
                void operator()(Rng &&, detail::from_end_<range_difference_type_t<Rng>>,
                    detail::from_end_<range_difference_type_t<Rng>>) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be sliced must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(SizedRange<Rng>() || ForwardRange<Rng>(),
                        "When slicing a range with a start and stop offset measured from the end, "
                        "the range must either be a model of the SizedRange concept (its size "
                        "must be known), or it must be a model of the ForwardRange concept.");
                }
                // slice(rng, 4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!(InputRange<Rng>()))>
                void operator()(Rng &&, range_difference_type_t<Rng>, end_fn) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be sliced must be a model of the InputRange concept.");
                }
                // slice(rng, end-4, end)
                template<typename Rng,
                    CONCEPT_REQUIRES_(!((InputRange<Rng>() && SizedRange<Rng>()) ||
                        ForwardRange<Rng>()))>
                void operator()(Rng &&, detail::from_end_<range_difference_type_t<Rng>>, end_fn) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be sliced must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(SizedRange<Rng>() || ForwardRange<Rng>(),
                        "When slicing a range with a start and stop offset measured from the end, "
                        "the range must either be a model of the SizedRange concept (its size "
                        "must be known), or it must be a model of the ForwardRange concept.");
                }
            #endif
            };

            /// \relates slice_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<slice_fn>, slice)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::slice_view)

#endif
