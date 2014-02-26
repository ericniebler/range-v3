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
            Distance
            distance_impl(InputIterator begin, Sentinel end, Distance d, InputIterator *pend,
                concepts::InputIterator)
            {
                for(; begin != end; ++begin)
                    ++d;
                if(pend)
                    *pend = begin;
                return d;
            }

            template<typename InputIterator, typename Sentinel, typename Distance>
            Distance
            distance_impl(InputIterator begin, Sentinel end, Distance d, InputIterator *pend,
                concepts::RandomAccessIterator)
            {
                d = end - begin;
                if(pend)
                    *pend = begin + d;
                return d;
            }

            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            distance(InputIterator begin, Sentinel end, InputIterator *pend = nullptr)
            {
                return detail::distance_impl(std::move(begin), std::move(end),
                    iterator_difference_t<InputIterator>{0}, pend,
                    iterator_concept_t<InputIterator>{});
            }

            template<typename InputIterator, typename Sentinel, typename Distance>
            Distance
            distance(InputIterator begin, Sentinel end, Distance d, InputIterator *pend = nullptr)
            {
                return detail::distance_impl(std::move(begin), std::move(end), d, pend,
                    iterator_concept_t<InputIterator>{});
            }
        }

        struct distance_ : bindable<distance_>, pipeable<distance_>
        {
            template<typename FiniteIterable,
                CONCEPT_REQUIRES_(ranges::FiniteIterable<FiniteIterable>())>
            static range_difference_t<FiniteIterable>
            invoke(distance_, FiniteIterable && rng)
            {
                return detail::distance(ranges::begin(rng), ranges::end(rng));
            }

            template<typename FiniteIterable, typename Distance,
                CONCEPT_REQUIRES_(ranges::FiniteIterable<FiniteIterable>())
              , CONCEPT_REQUIRES_(ranges::Integral<Distance>())
            >
            static Distance
            invoke(distance_, FiniteIterable && rng, Distance d)
            {
                return detail::distance(ranges::begin(rng), ranges::end(rng), d);
            }
        };

        RANGES_CONSTEXPR distance_ distance {};
    }
}

#endif
