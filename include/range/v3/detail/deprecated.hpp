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


#ifndef RANGES_DISABLE_DEPRECATED_WARNINGS
#if __cplusplus > 201103
#define RANGES_DEPRECATED(MSG) [[deprecated(MSG)]]
#else
#if defined(__clang__) || defined(__GNUC__)
#define RANGES_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#elif defined(_MSC_VER)
#define RANGES_DEPRECATED(MSG) __declspec(deprecated(MSG))
#else
#define RANGES_DEPRECATED(MSG)
#endif
#endif
#else
#define RANGES_DEPRECATED(MSG)
#endif
