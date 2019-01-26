/// \file
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
#ifndef RANGES_V3_FUNCTIONAL_COMPARISONS_HPP
#define RANGES_V3_FUNCTIONAL_COMPARISONS_HPP

#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    /// \addtogroup group-utility
    /// @{
    struct equal_to
    {
        template<typename T, typename U>
        constexpr auto operator()(T &&t, U &&u) const ->
            CPP_ret(bool)(
                requires EqualityComparableWith<T, U>)
        {
            return (T &&) t == (U &&) u;
        }
        using is_transparent = void;
    };

    struct not_equal_to
    {
        template<typename T, typename U>
        constexpr auto operator()(T &&t, U &&u) const ->
            CPP_ret(bool)(
                requires EqualityComparableWith<T, U>)
        {
            return (T &&) t != (U &&) u;
        }
        using is_transparent = void;
    };

    struct less
    {
        template<typename T, typename U>
        constexpr auto operator()(T &&t, U &&u) const ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<T, U>)
        {
            return (T &&) t < (U &&) u;
        }
        using is_transparent = void;
    };

    using ordered_less
        RANGES_DEPRECATED("Repace uses of ranges::less with ranges::less") =
            less;

}

#endif
