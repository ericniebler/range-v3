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

#ifndef RANGES_V3_ALGORITHM_TAGSPEC_HPP
#define RANGES_V3_ALGORITHM_TAGSPEC_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/tagged_pair.hpp>

namespace ranges
{
    inline namespace v3
    {
        RANGES_DEFINE_TAG_SPECIFIER(in)
        RANGES_DEFINE_TAG_SPECIFIER(in1)
        RANGES_DEFINE_TAG_SPECIFIER(in2)
        RANGES_DEFINE_TAG_SPECIFIER(out)
        RANGES_DEFINE_TAG_SPECIFIER(out1)
        RANGES_DEFINE_TAG_SPECIFIER(out2)
        RANGES_DEFINE_TAG_SPECIFIER(fun)
        RANGES_DEFINE_TAG_SPECIFIER(min)
        RANGES_DEFINE_TAG_SPECIFIER(max)
        RANGES_DEFINE_TAG_SPECIFIER(begin)
        RANGES_DEFINE_TAG_SPECIFIER(end)

        RANGES_DEFINE_TAG_SPECIFIER(current)
        RANGES_DEFINE_TAG_SPECIFIER(engine)
        RANGES_DEFINE_TAG_SPECIFIER(range)
        RANGES_DEFINE_TAG_SPECIFIER(size)
    }
}

#endif
