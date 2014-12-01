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
#ifndef RANGES_V3_UTILITY_IS_INFINITY_HPP
#define RANGES_V3_UTILITY_IS_INFINITY_HPP

#include <limits>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        template<typename T>
        constexpr bool is_infinity(T t)
        {
            return std::numeric_limits<T>::has_infinity &&
                t == std::numeric_limits<T>::infinity();
        }
        /// \endcond
    }
}

#endif
