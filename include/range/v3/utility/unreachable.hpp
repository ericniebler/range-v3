//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_UNREACHABLE_HPP
#define RANGES_V3_UTILITY_UNREACHABLE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
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

        // Specializations of common_type for EqualityComparison
        namespace concepts
        {
            template<typename T>
            struct common_type<T, unreachable>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<T, unreachable const>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<T, unreachable &>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<T, unreachable const &>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<unreachable, T>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<unreachable const, T>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<unreachable &, T>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
            template<typename T>
            struct common_type<unreachable const &, T>
            {
                using type =
                    basic_range_iterator<detail::common_range<detail::decay_t<T>, unreachable>>;
            };
        }
    }
}

namespace std
{
    template<typename T>
    struct common_type<T, ranges::unreachable>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<T, ranges::unreachable const>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<T, ranges::unreachable &>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<T, ranges::unreachable const &>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<ranges::unreachable, T>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<ranges::unreachable const, T>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<ranges::unreachable &, T>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
    template<typename T>
    struct common_type<ranges::unreachable const &, T>
    {
        using type =
            ranges::basic_range_iterator<ranges::detail::common_range<ranges::detail::decay_t<T>, ranges::unreachable>>;
    };
}

#endif
