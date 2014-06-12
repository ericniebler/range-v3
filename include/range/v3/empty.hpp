// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_EMPTY_HPP
#define RANGES_V3_EMPTY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct empty_fn
        {
            template<typename Rng>
            bool operator()(Rng &&rng) const
            {
                return begin(rng) == end(rng);
            }
        };

        RANGES_CONSTEXPR empty_fn empty {};
    }
}

#endif
