// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTLITY_ADDRESSOF_HPP
#define RANGES_V3_UTLITY_ADDRESSOF_HPP

#ifdef __cpp_lib_addressof_constexpr
#include <memory>
#endif

namespace ranges
{
    /// \cond
    namespace detail
    {
        template <typename T>
        constexpr T* addressof(T& arg) noexcept
        {
            #ifdef __cpp_lib_addressof_constexpr
            return std::addressof(arg);
            #else
            return &arg;
            #endif
        }

        template <typename T>
        T const* addressof(T const&&) = delete;
    }
    /// \endcond
}

#endif
