// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DETAIL_DISABLE_WARNINGS_HPP
#define RANGES_V3_DETAIL_DISABLE_WARNINGS_HPP

#include <range/v3/detail/config.hpp>

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_CXX2A_COMPAT

#ifdef RANGES_FEWER_WARNINGS
RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
RANGES_DIAGNOSTIC_IGNORE_INDENTATION
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
#endif

#endif // RANGES_V3_DETAIL_DISABLE_WARNINGS_HPP
