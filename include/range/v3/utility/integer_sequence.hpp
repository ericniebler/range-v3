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
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        ////////////////////////////////////////////////////////////////////////////////////////
        // index_t
        template<std::size_t N>
        using index_t = std::integral_constant<std::size_t, N>;

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
            template<typename I1, typename I2>
            struct concat_integer_sequence;

            template<typename T, T...N1, T...N2>
            struct concat_integer_sequence<integer_sequence<T, N1...>, integer_sequence<T, N2...>>
            {
                using type = integer_sequence<T, N1..., (sizeof...(N1) + N2)...>;
            };
        }

        // generate integer_sequence [0,N) in O(log(N)) time
        template<typename T, std::size_t N>
        struct make_integer_sequence
          : detail::concat_integer_sequence<
                typename make_integer_sequence<T, N / 2>::type,
                typename make_integer_sequence<T, N - N / 2>::type
            >
        {};

        template<typename T>
        struct make_integer_sequence<T, 0>
        {
            using type = integer_sequence<T>;
        };

        template<typename T>
        struct make_integer_sequence<T, 1>
        {
            using type = integer_sequence<T, 0>;
        };

        template<typename T, std::size_t N>
        using integer_sequence_t = typename make_integer_sequence<T, N>::type;

        template<std::size_t...Is>
        using index_sequence = integer_sequence<std::size_t, Is...>;

        template<std::size_t N>
        using index_sequence_t = integer_sequence_t<std::size_t, N>;
    }
}

#endif
