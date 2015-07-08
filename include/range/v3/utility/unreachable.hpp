/// \file
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
#ifndef RANGES_V3_UTILITY_UNREACHABLE_HPP
#define RANGES_V3_UTILITY_UNREACHABLE_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        struct unreachable
        {
            template<typename T>
            friend constexpr bool operator==(T const &, unreachable)
            {
                return false;
            }
            template<typename T>
            friend constexpr bool operator==(unreachable, T const &)
            {
                return false;
            }
            template<typename T>
            friend constexpr bool operator!=(T const &, unreachable)
            {
                return true;
            }
            template<typename T>
            friend constexpr bool operator!=(unreachable, T const &)
            {
                return true;
            }
            constexpr bool operator==(unreachable) const
            {
                return true;
            }
            constexpr bool operator!=(unreachable) const
            {
                return false;
            }
        };

        // Specializations of common_type for concept checking, needed because
        // std::common_type is not SFINAE-friendly.
        template<typename T>
        struct common_type<T, unreachable>
        {
            using type = common_iterator<T, unreachable>;
        };
        template<typename T>
        struct common_type<unreachable, T>
        {
            using type = common_iterator<T, unreachable>;
        };
        template<>
        struct common_type<unreachable, unreachable>
        {
            using type = unreachable;
        };

        template<typename T, typename TQual, typename UQual>
        struct basic_common_reference<T, unreachable, TQual, UQual>
        {
            using type = common_iterator<T, unreachable>;
        };
        template<typename T, typename TQual, typename UQual>
        struct basic_common_reference<unreachable, T, TQual, UQual>
        {
            using type = common_iterator<T, unreachable>;
        };
        template<typename TQual, typename UQual>
        struct basic_common_reference<unreachable, unreachable, TQual, UQual>
        {
            using type = decltype(true ? std::declval<meta::apply<TQual, unreachable>>()
                                       : std::declval<meta::apply<UQual, unreachable>>());
        };
        /// @}
    }
}

#endif
