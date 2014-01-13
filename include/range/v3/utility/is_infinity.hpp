//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_IS_INFINITY_HPP
#define RANGES_V3_UTILITY_IS_INFINITY_HPP

#include <limits>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        constexpr bool is_infinity(T t)
        {
            return std::numeric_limits<T>::has_infinity &&
                t == std::numeric_limits<T>::infinity();
        }
    }
}

#endif
