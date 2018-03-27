/// \file
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

#ifndef RANGES_V3_UTILITY_GET_HPP
#define RANGES_V3_UTILITY_GET_HPP

#include <utility>
#include <meta/meta.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename T>
        T & get(meta::id_t<T> & value)
        {
            return value;
        }

        template<typename T>
        T const & get(meta::id_t<T> const & value)
        {
            return value;
        }

        template<typename T>
        T && get(meta::id_t<T> && value)
        {
            return std::move(value);
        }
        /// @}
    }
}

#endif
