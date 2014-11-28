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

#ifndef RANGES_V3_BACK_HPP
#define RANGES_V3_BACK_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct back_fn
        {
            template<typename Rng,
                CONCEPT_REQUIRES_(BoundedIterable<Rng>() && BidirectionalIterable<Rng>())>
            range_reference_t<Rng> operator()(Rng &&rng) const
            {
                return *prev(end(rng));
            }
        };

        constexpr back_fn back {};
    }
}

#endif
