// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_CONTAINER_SORT_HPP
#define RANGES_V3_CONTAINER_SORT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/container/action.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace cont
        {
            struct sort_fn
            {
                template<typename Rng, CONCEPT_REQUIRES_(Sortable<range_iterator_t<Rng>>())>
                void operator()(Rng & rng) const
                {
                    ranges::sort(rng);
                }
            };

            RANGES_CONSTEXPR action<sort_fn> sort{};
        }
    }
}

#endif
