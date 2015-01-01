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
            struct difference_type<T, void_t<typename T::difference_type>>
            {
                using type = typename T::difference_type;
            };

            template<typename T>
            struct difference_type<T, typename std::enable_if<std::is_integral<T>::value>::type>
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
            struct value_type<T, void_t<typename T::value_type>>
              : std::enable_if<!std::is_void<typename T::value_type>::value, typename T::value_type>
            {
                // The use of enable_if is to accommodate output iterators that are
                // allowed to use void as their value type. We want treat output
                // iterators as non-Readable. value_type<OutIt> should be
                // SFINAE-friendly.
            };

            template<typename T>
            struct value_type<T, void_t<typename T::element_type>> // smart pointers
            {
                using type = typename T::element_type;
            };

            template<typename T>
            struct value_type<T, enable_if_t<std::is_base_of<std::ios_base, T>::value, void>>
            {
                using type = typename T::char_type;
            };

            template<typename T, typename Enable = void>
            struct iter_common_reference2
            {};

            template<typename T>
            struct iter_common_reference2<T,
                void_t<
                    common_reference_t<
                        remove_rvalue_reference_t<decltype(*std::declval<T>())> const &,
                        typename value_type<T>::type &>>>
            {
                using type =
                    common_reference_t<
                        remove_rvalue_reference_t<decltype(*std::declval<T>())> const &,
                        typename value_type<T>::type &>;
            };

            template<typename T, typename Enable = void>
            struct iter_common_reference
              : iter_common_reference2<T>
            {};

            template<typename T>
            struct iter_common_reference<T, void_t<typename T::common_reference>>
            {
                using type = typename T::common_reference;
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
          : std::make_unsigned<meta::eval<difference_type<T>>>
        {};

        template<typename T>
        struct value_type
          : detail::value_type<uncvref_t<T>>
        {};

        template<typename T>
        struct iter_common_reference
          : detail::iter_common_reference<uncvref_t<T>>
        {};
        /// @}
    }
}

#endif
