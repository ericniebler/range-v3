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
#ifndef RANGES_V3_UTILITY_UNREACHABLE_HPP
#define RANGES_V3_UTILITY_UNREACHABLE_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        struct unreachable
        {
            template<typename T>
            friend constexpr bool operator==(T const &, unreachable)
            {
                return false;
            }
            template<typename T>
            friend constexpr bool operator==(unreachable, T const &)
            {
                return false;
            }
            template<typename T>
            friend constexpr bool operator!=(T const &, unreachable)
            {
                return true;
            }
            template<typename T>
            friend constexpr bool operator!=(unreachable, T const &)
            {
                return true;
            }
        };
        /// @}
    }
}

#endif
