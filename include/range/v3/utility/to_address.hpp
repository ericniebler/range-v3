/// \file
// Range v3 library
//
//  Copyright Glen Joseph Fernandes 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef RANGES_V3_UTILITY_TO_ADDRESS_HPP
#define RANGES_V3_UTILITY_TO_ADDRESS_HPP

#include <memory>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility Utility
        /// @{
        template<typename T>
        typename std::pointer_traits<T>::element_type* to_address(T p) noexcept
        {
            return ranges::to_address(p.operator->());
        }

        template<typename T>
        T* to_address(T* p) noexcept
        {
            return p;
        }
        /// @}
    }
}

#endif
