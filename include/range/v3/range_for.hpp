// Range v3 library
//
//  Copyright Eric Niebler 2014
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

// A range-based for macro, basically a hack until the build-in range-for can handle Iterables
// which have a different type for begin and end
#define RANGES_FOR(VAR_DECL, RANGE)                                                                  \
    if(bool _range_v3_done = false) {}                                                              \
    else for(auto && _range_v3_rng = (RANGE); !_range_v3_done;)                                     \
        for(auto _range_v3_begin = ranges::begin(_range_v3_rng); !_range_v3_done;)                  \
            for(auto _range_v3_end = ranges::end(_range_v3_rng);                                    \
                _range_v3_begin != _range_v3_end || !(_range_v3_done = true); ++_range_v3_begin)    \
                if(bool _range_v3_once = false) {}                                                  \
                else for(VAR_DECL = *_range_v3_begin; !_range_v3_once; _range_v3_once = true)       \
    /**/

#endif
