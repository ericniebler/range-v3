// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_INTEGER_SEQUENCE_HPP
#define RANGES_V3_UTILITY_INTEGER_SEQUENCE_HPP

#include <cstddef>
#include <type_traits>
#include <range/v3/detail/config.hpp>
#include <range/v3/utility/meta.hpp>

namespace ranges
{
    inline namespace v3
    {
        ////////////////////////////////////////////////////////////////////////////////////////
        // integer_sequence
        template<typename T, T...Is>
        struct integer_sequence
        {
            using value_type = T;
            static constexpr std::size_t size() noexcept
            {
                return sizeof...(Is);
            }
        };

        namespace detail
        {
            // Glue two sets of integer_sequence together
            template<typename I1, typename I2, typename I3>
            struct integer_sequence_cat;

            template<typename T, T...N1, T...N2, T...N3>
            struct integer_sequence_cat<integer_sequence<T, N1...>, integer_sequence<T, N2...>,
                integer_sequence<T, N3...>>
            {
                using type = integer_sequence<T, N1..., (sizeof...(N1) + N2)...,
                    (sizeof...(N1) + sizeof...(N2) + N3)...>;
            };

            template<typename T, std::size_t N>
            struct make_integer_sequence_
              : integer_sequence_cat<
                    meta::eval<make_integer_sequence_<T, N / 2>>,
                    meta::eval<make_integer_sequence_<T, N / 2>>,
                    meta::eval<make_integer_sequence_<T, N % 2>>>
            {};

            template<typename T>
            struct make_integer_sequence_<T, 0>
            {
                using type = integer_sequence<T>;
            };

            template<typename T>
            struct make_integer_sequence_<T, 1>
            {
                using type = integer_sequence<T, 0>;
            };
        }

        // generate integer_sequence [0,N) in O(log(N)) time
        template<typename T, T N>
        using make_integer_sequence =
            meta::eval<detail::make_integer_sequence_<T, (std::size_t)N>>;

        template<std::size_t...Is>
        using index_sequence = integer_sequence<std::size_t, Is...>;

        template<std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;
    }
}

#endif
