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

#ifndef RANGES_V3_ACTION_UNIQUE_HPP
#define RANGES_V3_ACTION_UNIQUE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace action
        {
            struct unique_fn
            {
                template<typename Rng, typename I = range_iterator_t<Rng>,
                    CONCEPT_REQUIRES_(Sortable<I>() && EraseableIterable<Rng &, I, I>())>
                void operator()(Rng & rng) const
                {
                    auto it = ranges::unique(rng);
                    ranges::erase(rng, it, end(rng));
                }
            };

            constexpr action<unique_fn> unique{};
        }
    }
}

#endif
