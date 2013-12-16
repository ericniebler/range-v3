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

namespace ranges
{
    inline namespace v3
    {
        struct range_swapper : bindable<range_swapper>
        {
            /// \overload
            template<typename ForwardRange1, typename ForwardRange2>
            static range_iterator_t<ForwardRange2>
            invoke(range_swapper, ForwardRange1 && rng1, ForwardRange2 && rng2)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2>());
                return std::swap_ranges(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2));
            }
        };

        RANGES_CONSTEXPR range_swapper swap_ranges {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
