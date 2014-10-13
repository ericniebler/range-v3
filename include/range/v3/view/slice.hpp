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
#include <range/v3/view/counted.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/counted_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Rng, bool IsRandomAccess = RandomAccessIterable<Rng>()>
            struct sliced_view_
              : range_facade<sliced_view<Rng>, false>
            {
            private:
                friend range_access;
                using base_range_t = view::all_t<Rng>;
                using difference_type_ = range_difference_t<Rng>;
                base_range_t rng_;
                difference_type_ from_, to_;

                detail::counted_cursor<range_iterator_t<Rng>> begin_cursor()
                {
                    return {next(ranges::begin(rng_), from_), to_ - from_};
                }
                CONCEPT_REQUIRES(Iterable<Rng const>())
                detail::counted_cursor<range_iterator_t<Rng const>> begin_cursor() const
                {
                    return {next(ranges::begin(rng_), from_), to_ - from_};
                }
                detail::counted_sentinel end_cursor() const
                {
                    return {};
                }
            public:
                sliced_view_() = default;
                sliced_view_(Rng && rng, difference_type_ from, difference_type_ to)
                  : rng_(view::all(std::forward<Rng>(rng))), from_(from), to_(to)
                {
                    RANGES_ASSERT(from >= 0);
                    RANGES_ASSERT(to >= from);
                }
                range_size_t<Rng> size() const
                {
                    return static_cast<range_size_t<Rng>>(to_ - from_);
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
                difference_type_ from_, to_;
            public:
                sliced_view_() = default;
                sliced_view_(Rng && rng, difference_type_ from, difference_type_ to)
                  : rng_(view::all(std::forward<Rng>(rng))), from_(from), to_(to)
                {
                    RANGES_ASSERT(from >= 0);
                    RANGES_ASSERT(to >= from);
                }
                range_iterator_t<Rng> begin()
                {
                    return next(ranges::begin(rng_), from_);
                }
                range_iterator_t<Rng> end()
                {
                    return next(ranges::begin(rng_), to_);
                }
                CONCEPT_REQUIRES(Iterable<Rng const>())
                range_iterator_t<Rng const> begin() const
                {
                    return next(ranges::begin(rng_), from_);
                }
                CONCEPT_REQUIRES(Iterable<Rng const>())
                range_iterator_t<Rng const> end() const
                {
                    return next(ranges::begin(rng_), to_);
                }
                range_size_t<Rng> size() const
                {
                    return static_cast<range_size_t<Rng>>(to_ - from_);
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

        template<typename Rng>
        struct sliced_view
          : detail::sliced_view_<Rng>
        {
            using detail::sliced_view_<Rng>::sliced_view_;
        };

        namespace view
        {
            struct slice_fn : bindable<slice_fn>
            {
            private:
                template<typename Rng>
                static sliced_view<Rng>
                invoke_(Rng && rng, range_difference_t<Rng> from, range_difference_t<Rng> to, concepts::InputIterable*)
                {
                    return {std::forward<Rng>(rng), from, to};
                }
                template<typename Rng, CONCEPT_REQUIRES_(!Range<Rng>() && std::is_lvalue_reference<Rng>())>
                static range<range_iterator_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> from, range_difference_t<Rng> to, concepts::RandomAccessIterable*)
                {
                    return {next(begin(rng), from), next(begin(rng), to)};
                }
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputIterable<Rng>())>
                static auto
                invoke(slice_fn, Rng && rng, range_difference_t<Rng> from, range_difference_t<Rng> to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    slice_fn::invoke_(std::forward<Rng>(rng), from, to, iterable_concept<Rng>{})
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto
                invoke(slice_fn slice, Int from, Int to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    slice.move_bind(std::placeholders::_1, (Int)from, (Int)to)
                )
            };

            RANGES_CONSTEXPR slice_fn slice {};
        }
    }
}

#endif
