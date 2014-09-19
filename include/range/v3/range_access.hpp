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

#ifndef RANGES_V3_RANGE_ACCESS_HPP
#define RANGES_V3_RANGE_ACCESS_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct front_fn
        {
            template<typename Rng,
                CONCEPT_REQUIRES_(Iterable<Rng>())>
            range_reference_t<Rng> operator()(Rng &&rng) const
            {
                return *begin(rng);
            }
        };

        RANGES_CONSTEXPR front_fn front {};

        struct back_fn
        {
            template<typename Rng,
                CONCEPT_REQUIRES_(BoundedIterable<Rng>() && BidirectionalIterable<Rng>())>
            range_reference_t<Rng> operator()(Rng &&rng) const
            {
                return *prev(end(rng));
            }
        };

        RANGES_CONSTEXPR back_fn back {};

        struct at_fn
        {
            template<typename Rng,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>())>
            auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
                decltype(begin(rng)[n])
            {
                return begin(rng)[n];
            }
        };

        RANGES_CONSTEXPR at_fn at {};
    }
}

#endif
