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

#ifndef RANGES_V3_SIZE_HPP
#define RANGES_V3_SIZE_HPP

#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct size_ : bindable<size_>
        {
            template<typename RandomAccessRange>
            range_difference_t<RandomAccessRange> operator()(RandomAccessRange && rng) const
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                return ranges::end(rng) - ranges::begin(rng);
            }
        };

        RANGES_CONSTEXPR size_ size {};
    }
}

#endif
