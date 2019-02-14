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
    /// \addtogroup group-functional
    /// @{
    struct equal_to
    {
        CPP_template(typename T, typename U)(
            requires EqualityComparableWith<T, U>)
        constexpr bool operator()(T &&t, U &&u) const
        {
            return (T &&) t == (U &&) u;
        }
        using is_transparent = void;
    };

    struct not_equal_to
    {
        CPP_template(typename T, typename U)(
            requires EqualityComparableWith<T, U>)
        constexpr bool operator()(T &&t, U &&u) const
        {
            return !equal_to{}((T &&) t, (U &&) u);
        }
        using is_transparent = void;
    };

    struct less
    {
        CPP_template(typename T, typename U)(
            requires StrictTotallyOrderedWith<T, U>)
        constexpr bool operator()(T &&t, U &&u) const
        {
            return (T &&) t < (U &&) u;
        }
        using is_transparent = void;
    };

    struct less_equal
    {
        template<typename T, typename U>
        constexpr auto operator()(T &&t, U &&u) const ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<T, U>)
        {
            return !less{}((U &&) u, (T &&) t);
        }
        using is_transparent = void;
    };

    struct greater_equal
    {
        template<typename T, typename U>
        constexpr auto operator()(T &&t, U &&u) const ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<T, U>)
        {
            return !less{}((T &&) t, (U &&) u);
        }
        using is_transparent = void;
    };

    struct greater
    {
        template<typename T, typename U>
        constexpr auto operator()(T &&t, U &&u) const ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<T, U>)
        {
            return less{}((U &&) u, (T &&) t);
        }
        using is_transparent = void;
    };

    using ordered_less
        RANGES_DEPRECATED("Repace uses of ranges::ordered_less with ranges::less") =
            less;

    namespace cpp20
    {
        using ranges::equal_to;
        using ranges::not_equal_to;
        using ranges::less;
        using ranges::less_equal;
        using ranges::greater;
        using ranges::greater_equal;
    }
    /// @}
}

#endif
