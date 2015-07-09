/// \file
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

#ifndef RANGES_V3_UTILITY_META_HPP
#define RANGES_V3_UTILITY_META_HPP

#if defined(__GNUC__) || defined(__clang__)
#  pragma message "This header is deprecated. Please use: meta/meta.hpp"
#endif

#include <range/v3/range_fwd.hpp>
#include <meta/meta.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace meta = ::meta::v1;
    }
}

#endif
