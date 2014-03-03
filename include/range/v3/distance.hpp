// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_DISTANCE_HPP
#define RANGES_V3_DISTANCE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/range_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // distance_impl
            template<typename InputIterator, typename Sentinel, typename Distance>
            std::pair<Distance, InputIterator>
            distance_impl(InputIterator begin, Sentinel end, Distance d,
                concepts::InputIterator)
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }

            template<typename RandomAccessIterator, typename Distance>
            std::pair<Distance, RandomAccessIterator>
            distance_impl(RandomAccessIterator begin, RandomAccessIterator end, Distance d,
                concepts::RandomAccessIterator)
            {
                return {end - begin, end};
            }

            template<typename InputIterator, typename Sentinel,
                typename Distance = iterator_difference_t<InputIterator>>
            std::pair<Distance, InputIterator>
            distance(InputIterator begin, Sentinel end, Distance d = Distance{0})
            {
                return detail::distance_impl(std::move(begin), std::move(end), d,
                    iterator_concept_t<InputIterator>{});
            }
        }

        struct distance_ : bindable<distance_>, pipeable<distance_>
        {
        private:
            template<typename FiniteIterable, typename Distance>
            static Distance
            impl(FiniteIterable && rng, Distance d, concepts::Iterable)
            {
                return detail::distance(ranges::begin(rng), ranges::end(rng), d).first;
            }
            template<typename FiniteIterable, typename Distance>
            static Distance
            impl(FiniteIterable && rng, Distance d, concepts::CountedIterable)
            {
                return Distance{ranges::end(rng).count() - ranges::begin(rng).count()} + d;
            }
        public:
            template<typename FiniteIterable,
                typename Distance = range_difference_t<FiniteIterable>,
                CONCEPT_REQUIRES_(ranges::FiniteIterable<FiniteIterable>() &&
                                  ranges::Integral<Distance>())
            >
            static Distance
            invoke(distance_, FiniteIterable && rng, Distance d = Distance{0})
            {
                return distance_::impl(std::forward<FiniteIterable>(rng), d,
                    range_concept_t<FiniteIterable>{});
            }
        };

        RANGES_CONSTEXPR distance_ distance {};
    }
}

#endif
