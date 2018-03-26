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

#ifndef RANGES_V3_RANGE_FOR_HPP
#define RANGES_V3_RANGE_FOR_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>

#if RANGES_CXX_RANGE_BASED_FOR < RANGES_CXX_RANGE_BASED_FOR_17
/// A range-based for macro, basically a hack until the build-in range-for can handle Ranges
/// which have a different type for begin and end.
/// \ingroup range-core
#define RANGES_FOR(VAR_DECL, ...)                                                               \
    if(bool _range_v3_done = false) {}                                                          \
    else for(auto && _range_v3_rng = (__VA_ARGS__); !_range_v3_done;)                           \
        for(auto _range_v3_begin = ranges::begin(_range_v3_rng); !_range_v3_done;               \
                _range_v3_done = true)                                                          \
            for(auto _range_v3_end = ranges::end(_range_v3_rng);                                \
                    !_range_v3_done && _range_v3_begin != _range_v3_end; ++_range_v3_begin)     \
                if(!(_range_v3_done = true)) {}                                                 \
                else for(VAR_DECL = *_range_v3_begin; _range_v3_done; _range_v3_done = false)   \
    /**/

#else
#define RANGES_FOR(VAR_DECL, ...) for(VAR_DECL : (__VA_ARGS__))
#endif

#endif
