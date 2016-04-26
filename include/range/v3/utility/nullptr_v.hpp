/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013,2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_NULLVAL_HPP
#define RANGES_V3_UTILITY_NULLVAL_HPP

#include <range/v3/detail/config.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-utility
        template<typename T>
        constexpr T *_nullptr_v()
        {
            return nullptr;
        }

    #if RANGES_CXX_VARIABLE_TEMPLATES
        /// \ingroup group-utility
        template<typename T>
        constexpr T *nullptr_v = nullptr;
    #endif
    }
}

#endif
