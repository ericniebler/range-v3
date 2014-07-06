// Boost.Range library
//
//  Copyright Eric Niebler 2013,2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_NULLVAL_HPP
#define RANGES_V3_UTILITY_NULLVAL_HPP

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        constexpr T *nullval()
        {
            return nullptr;
        }
    }
}

#endif
