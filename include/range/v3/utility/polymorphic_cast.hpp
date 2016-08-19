//  (C) Copyright Kevlin Henney and Dave Abrahams 1999.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP
#define RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP

namespace ranges
{
    inline namespace v3
    {
        template<typename Target, typename Source>
        inline Target polymorphic_downcast(Source* x)
        {
            RANGES_ASSERT(dynamic_cast<Target>(x) == x);
            return static_cast<Target>(x);
        }
    }
}

#endif  // RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP
