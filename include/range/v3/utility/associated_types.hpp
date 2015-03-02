/// \file
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

#ifndef RANGES_V3_UTILITY_ASSOCIATED_TYPES_HPP
#define RANGES_V3_UTILITY_ASSOCIATED_TYPES_HPP

#include <iosfwd>
#include <cstddef>
#include <utility>
#include <type_traits>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/common_type.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename Enable = void>
            struct difference_type
            {};

            template<>
            struct difference_type<std::nullptr_t, void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T>
            struct difference_type<T *, void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T>
            struct difference_type<T[], void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T, std::size_t N>
            struct difference_type<T[N], void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T>
            struct difference_type<T, meta::void_<typename T::difference_type>>
            {
                using type = typename T::difference_type;
            };

            template<typename T>
            struct difference_type<T, meta::if_<std::is_integral<T>>>
            {
                using type = decltype(std::declval<T>() - std::declval<T>());
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename Enable = void>
            struct value_type
            {};

            template<typename T>
            struct value_type<T *, void>
              : std::remove_cv<T>
            {};

            template<typename T>
            struct value_type<T[], void>
              : std::remove_cv<T>
            {};

            template<typename T, std::size_t N>
            struct value_type<T[N], void>
              : std::remove_cv<T>
            {};

            template<typename T>
            struct value_type<T, meta::void_<typename T::value_type>>
              : meta::lazy::if_<meta::not_<std::is_void<typename T::value_type>>,
                    typename T::value_type>
            {
                // The use of meta::if_c is to accommodate output iterators that are
                // allowed to use void as their value type. We want treat output
                // iterators as non-Readable. value_type<OutIt> should be
                // SFINAE-friendly.
            };

            template<typename T>
            struct value_type<T, meta::void_<typename T::element_type>> // smart pointers
              : meta::lazy::if_<meta::not_<std::is_void<typename T::element_type>>,
                    typename T::element_type>
            {
                // The meta::if_c here is because shared_ptr<void> is not Readable.
            };

            template<typename T>
            struct value_type<T, meta::if_<std::is_base_of<std::ios_base, T>>>
            {
                using type = typename T::char_type;
            };
        }
        /// \endcond

        ////////////////////////////////////////////////////////////////////////////////////////////
        //
        template<typename T>
        struct difference_type
          : detail::difference_type<uncvref_t<T>>
        {};

        template<typename T>
        struct size_type
          : meta::lazy::let<std::make_unsigned<meta::lazy::eval<difference_type<T>>>>
        {};

        template<typename T>
        struct value_type
          : detail::value_type<uncvref_t<T>>
        {};
        /// @}
    }
}

#endif
