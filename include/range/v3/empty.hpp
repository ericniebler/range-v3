// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_EMPTY_HPP
#define RANGES_V3_EMPTY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct empty_fn
        {
            template<typename Rng,
                CONCEPT_REQUIRES_(Iterable<Rng>())>
            bool operator()(Rng &&rng) const
            {
                return begin(rng) == end(rng);
            }
        };

        constexpr empty_fn empty {};
    }
}

#endif
