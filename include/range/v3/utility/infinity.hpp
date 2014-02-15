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

#ifndef RANGES_V3_UTILITY_INFINITY_HPP
#define RANGES_V3_UTILITY_INFINITY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct infinity
        {
        };

        constexpr bool operator==(infinity, infinity)
        {
            return true;
        }
        constexpr bool operator!=(infinity, infinity)
        {
            return false;
        }

        template<typename Integer,
            CONCEPT_REQUIRES(ranges::Integral<Integer>())>
        constexpr bool operator==(Integer, infinity)
        {
            return false;
        }
        template<typename Integer,
            CONCEPT_REQUIRES(ranges::Integral<Integer>())>
        constexpr bool operator==(infinity, Integer)
        {
            return false;
        }
        template<typename Integer,
            CONCEPT_REQUIRES(ranges::Integral<Integer>())>
        constexpr bool operator!=(Integer, infinity)
        {
            return true;
        }
        template<typename Integer,
            CONCEPT_REQUIRES(ranges::Integral<Integer>())>
        constexpr bool operator!=(infinity, Integer)
        {
            return true;
        }
    }
}

#endif
