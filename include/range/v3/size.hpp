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

#ifndef RANGES_V3_SIZE_HPP
#define RANGES_V3_SIZE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/range_concepts.hpp> // defines range_size
#include <range/v3/range_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Iterable>
        using range_size_t = meta_apply<std::make_unsigned, range_difference_t<Iterable>>;

        struct size_fn
        {
            template<typename Iterable,
                CONCEPT_REQUIRES_(ranges::SizedIterable<Iterable>())>
            constexpr auto operator()(Iterable const &rng) const -> decltype(range_size(rng))
            {
                return range_size(rng);
            }
        };

        RANGES_CONSTEXPR size_fn size {};
    }
}

#endif
