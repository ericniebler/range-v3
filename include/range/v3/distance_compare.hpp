// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Michel Morin 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DISTANCE_COMPARE_HPP
#define RANGES_V3_DISTANCE_COMPARE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        // The interface of distance_compare is taken from Util.listLengthCmp in the GHC API.
        struct distance_compare_fn
        {
        private:
            template<typename Rng>
            int impl_r(Rng &rng, range_difference_t<Rng> n, concepts::Iterable*) const
            {
                if (n >= 0) {
                    auto it = rng.begin();
                    auto end = rng.end();
                    for (; n > 0; --n) {
                        if (it == end) {
                            return -1;
                        }
                        ++it;
                    }
                    return it == end ? 0 : 1;
                }
                else {
                    return 1;
                }
            }
            template<typename Rng>
            int impl_r(Rng &rng, range_difference_t<Rng> n, concepts::SizedIterable*) const
            {
                range_difference_t<Rng> dist = static_cast<range_difference_t<Rng>>(size(rng));
                if (dist > n)
                    return  1;
                else if (dist < n)
                    return -1;
                else
                    return  0;
            }
        public:
            template<typename Rng,
                CONCEPT_REQUIRES_(Iterable<Rng>())>
            int operator()(Rng &&rng, range_difference_t<Rng> n) const
            {
                return this->impl_r(rng, n, sized_iterable_concept<Rng>());
            }
        };

        RANGES_CONSTEXPR distance_compare_fn distance_compare {};
    }
}

#endif
