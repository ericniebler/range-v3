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

#ifndef RANGES_V3_DETAIL_DISABLE_WARNINGS_HPP
#define RANGES_V3_DETAIL_DISABLE_WARNINGS_HPP
#include <range/v3/detail/config.hpp>
#endif

#ifdef RANGES_DISABLE_WARNINGS_INCLUDED
#error "Warnings already disabled!"
#endif
#define RANGES_DISABLE_WARNINGS_INCLUDED

RANGES_DIAGNOSTIC_PUSH

#ifdef RANGES_FEWER_WARNINGS
RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
RANGES_DIAGNOSTIC_IGNORE_INDENTATION
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
#endif
