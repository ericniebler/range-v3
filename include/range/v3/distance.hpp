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
#include <range/v3/utility/is_infinity.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // distance_impl
            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            distance_impl(InputIterator begin, Sentinel end, InputIterator *pend,
                concepts::InputIterator)
            {
                iterator_difference_t<InputIterator> n = 0;
                for(; begin != end; ++begin)
                    ++n;
                if(pend)
                    *pend = begin;
                return n;
            }

            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            distance_impl(InputIterator begin, Sentinel end, InputIterator *pend,
                concepts::RandomAccessIterator)
            {
                iterator_difference_t<InputIterator> n = end - begin;
                if(pend)
                {
                    RANGES_ASSERT(!ranges::is_infinity(n));
                    *pend = begin + n;
                }
                return n;
            }

            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            distance(InputIterator begin, Sentinel end, InputIterator *pend = 0)
            {
                return detail::distance_impl(std::move(begin), std::move(end), pend,
                    iterator_concept_t<InputIterator>{});
            }
        }

        struct distance_ : bindable<distance_>, pipeable<distance_>
        {
            template<typename Range>
            static range_difference_t<Range> invoke(distance_, Range && rng)
            {
                return detail::distance(ranges::begin(rng), ranges::end(rng));
            }
        };

        RANGES_CONSTEXPR distance_ distance {};
    }
}

#endif
