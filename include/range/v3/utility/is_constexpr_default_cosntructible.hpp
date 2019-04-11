/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_IS_CONSTEXPR_DEFAULT_COSNTRUCTIBLE_HPP
#define RANGE_V3_IS_CONSTEXPR_DEFAULT_COSNTRUCTIBLE_HPP

#include <type_traits>

namespace ranges
{
    inline namespace v3
    {
        // Rationale. Source https://stackoverflow.com/a/46920091 :
        // --------------------------------------------------------
        // is_constexpr_helper is constexpr, so it will be a constant expression as long as its argument is.
        // If it's a constant expression, it will be noexcept, but otherwise it won't be
        // (since it isn't marked as such).

        namespace details
        {
            template<class T>
            constexpr void is_constexpr_helper()
                noexcept/*(std::is_nothrow_default_constructible<T>::value)*/ {}
        }

        template <typename T, typename Enable = void>
        struct is_constexpr_default_cosntructible : std::false_type {};

        template <typename T>
        struct is_constexpr_default_cosntructible<T, typename std::enable_if<std::is_default_constructible<T>::value>::type >
            : std::integral_constant<bool, noexcept(details::is_constexpr_helper<T>())> {};
    }
}

#endif //RANGE_V3_IS_CONSTEXPR_DEFAULT_COSNTRUCTIBLE_HPP
