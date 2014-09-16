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
        // size_t
        template<std::size_t N>
        using size_t = std::integral_constant<std::size_t, N>;

        ////////////////////////////////////////////////////////////////////////////////////////
        // integer_sequence
        template<std::size_t ...Is>
        struct integer_sequence
        {
            using value_type = std::size_t;
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

            template<std::size_t...N1, std::size_t...N2>
            struct concat_integer_sequence<integer_sequence<N1...>, integer_sequence<N2...>>
            {
                using type = integer_sequence<N1..., (sizeof...(N1) + N2)...>;
            };
        }

        // generate integer_sequence [0,N) in O(log(N)) time
        template<std::size_t N>
        struct make_integer_sequence
          : detail::concat_integer_sequence<
                typename make_integer_sequence<N / 2>::type
              , typename make_integer_sequence<N - N / 2>::type
            >
        {};

        template<>
        struct make_integer_sequence<0>
        {
            using type = integer_sequence<>;
        };

        template<>
        struct make_integer_sequence<1>
        {
            using type = integer_sequence<0>;
        };

        template<std::size_t N>
        using integer_sequence_t = typename make_integer_sequence<N>::type;
    }
}

#endif
