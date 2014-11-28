// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements: Thanks for Paul Fultz for the suggestions that
//                   concepts can be ordinary Boolean metafunctions.
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_COMMON_TYPE_HPP
#define RANGES_V3_UTILITY_COMMON_TYPE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>

// Sadly, this is necessary because of:
//  - std::common_type is not SFINAE-friendly, and
//  - The specification of std::common_type makes it impossible
//    difficult to specialize on user-defined types without spamming
//    out a bajillion copies to handle all combinations of cv and ref
//    qualifiers.

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T, typename U>
            using default_common_t = decltype(true? std::declval<T>() : std::declval<U>());

            template<typename T, typename U, typename Enable = void>
            struct common_type_if
            {};

            template<typename T, typename U>
            struct common_type_if<T, U, void_t<default_common_t<T, U>>>
            {
                using type = decay_t<default_common_t<T, U>>;
            };

            template<typename T, typename U,
                     typename TT = decay_t<T>, typename UU = decay_t<U>>
            struct common_type2
              : common_type<TT, UU> // Recurse to catch user specializations
            {};

            template<typename T, typename U>
            struct common_type2<T, U, T, U>
              : common_type_if<T, U>
            {};

            template<typename Meta, typename Enable = void>
            struct has_type
              : std::false_type
            {};

            template<typename Meta>
            struct has_type<Meta, void_t<typename Meta::type>>
              : std::true_type
            {};

            template<typename Meta, typename...Ts>
            struct common_type_recurse
              : common_type<typename Meta::type, Ts...>
            {};

            template<typename Meta, typename...Ts>
            struct common_type_recurse_if
              : meta::if_<
                    has_type<Meta>,
                    common_type_recurse<Meta, Ts...>,
                    empty>
            {};
        }

        // Users should specialize this to hook the Common concept
        // until std gets a SFINAE-friendly std::common_type and there's
        // some sane way to deal with cv and ref qualifiers.
        template<typename ...Ts>
        struct common_type
        {};

        template<typename T>
        struct common_type<T>
        {
            using type = detail::decay_t<T>;
        };

        template<typename T, typename U>
        struct common_type<T, U>
          : detail::common_type2<T, U>
        {};

        template<typename T, typename U, typename... Vs>
        struct common_type<T, U, Vs...>
          : detail::common_type_recurse_if<common_type<T, U>, Vs...>
        {};
    }
}

#endif
