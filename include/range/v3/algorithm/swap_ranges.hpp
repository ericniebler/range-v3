//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_SWAP_RANGES_HPP
#define RANGES_V3_ALGORITHM_SWAP_RANGES_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator1, typename Sentinel1,
                     typename ForwardIterator2, typename Sentinel2>
            ForwardIterator2
            swap_ranges(ForwardIterator1 begin1, Sentinel1 end1,
                        ForwardIterator2 begin2, Sentinel2 end2)
            {
                for(; begin1 != end1; ++begin1, ++begin2)
                {
                    RANGE_ASSERT(begin2 != end2);
                    ranges::swap(*begin1, *begin2);
                }
                return begin2;
            }
        }

        struct range_swapper : bindable<range_swapper>
        {
            /// \overload
            template<typename ForwardIterable1, typename ForwardIterable2>
            static range_iterator_t<ForwardIterable2>
            invoke(range_swapper, ForwardIterable1 && rng1, ForwardIterable2 && rng2)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2>());
                CONCEPT_ASSERT(ranges::Swappable<range_reference_t<ForwardIterable1>,
                                                 range_reference_t<ForwardIterable2>>());
                return detail::swap_ranges(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2));
            }
        };

        RANGES_CONSTEXPR range_swapper swap_ranges {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
