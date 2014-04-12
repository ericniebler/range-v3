//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_FUNCTIONAL_HPP
#define RANGES_V3_UTILITY_FUNCTIONAL_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct equal_to
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t == (U &&) u;
            }
        };

        struct less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(WeaklyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
        };

        struct ordered_less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
        };

        struct ident
        {
            template<typename T>
            T && operator()(T && t) const
            {
                return (T &&) t;
            }
        };
    }
}

#endif
