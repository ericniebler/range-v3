/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_INFINITY_HPP
#define RANGES_V3_UTILITY_INFINITY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
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
            CONCEPT_REQUIRES_(Integral<Integer>())>
        constexpr bool operator==(Integer, infinity)
        {
            return false;
        }
        template<typename Integer,
            CONCEPT_REQUIRES_(Integral<Integer>())>
        constexpr bool operator==(infinity, Integer)
        {
            return false;
        }
        template<typename Integer,
            CONCEPT_REQUIRES_(Integral<Integer>())>
        constexpr bool operator!=(Integer, infinity)
        {
            return true;
        }
        template<typename Integer,
            CONCEPT_REQUIRES_(Integral<Integer>())>
        constexpr bool operator!=(infinity, Integer)
        {
            return true;
        }
        /// \endcond
    }
}

#endif
