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
#include <meta/meta.hpp>
#include <range/v3/utility/common_type.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        /// @{

        ////////////////////////////////////////////////////////////////////////////////////////
        template<typename T, typename Enable /*= void*/>
        struct difference_type
        {};

        template<>
        struct difference_type<std::nullptr_t>
        {
            using type = std::ptrdiff_t;
        };

        template<typename T>
        struct difference_type<T *>
          : meta::lazy::if_<std::is_object<T>, std::ptrdiff_t>
        {};

        template<typename T>
        struct difference_type<T, meta::if_<std::is_array<T>>>
          : difference_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct difference_type<T const>
          : difference_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct difference_type<T volatile>
          : difference_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct difference_type<T const volatile>
          : difference_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct difference_type<T, meta::void_<typename T::difference_type>>
        {
            using type = typename T::difference_type;
        };

        template<typename T>
        struct difference_type<T, meta::if_<std::is_integral<T>>>
          : std::make_signed<decltype(std::declval<T>() - std::declval<T>())>
        {};

        ////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        struct size_type
          : meta::lazy::let<std::make_unsigned<meta::lazy::eval<difference_type<T>>>>
        {};

        ////////////////////////////////////////////////////////////////////////////////////////
        template<typename T, typename Enable /*= void*/>
        struct value_type
        {};

        template<typename T>
        struct value_type<T *>
          : meta::lazy::if_<std::is_object<T>, meta::_t<std::remove_cv<T>>>
        {
            // The meta::lazy::if_ is because void* and void(*)() are not Readable.
        };

        template<typename T>
        struct value_type<T, meta::if_<std::is_array<T>>>
          : value_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct value_type<T const>
          : value_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct value_type<T volatile>
          : value_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct value_type<T const volatile>
          : value_type<detail::decay_t<T>>
        {};

        template<typename T>
        struct value_type<T, meta::void_<typename T::value_type>>
          : meta::lazy::if_<std::is_object<typename T::value_type>, typename T::value_type>
        {
            // The meta::lazy::if_ is to accommodate output iterators that are
            // allowed to use void as their value type. We want treat output
            // iterators as non-Readable. value_type<OutIt> should be
            // SFINAE-friendly.
        };

        template<typename T>
        struct value_type<T, meta::void_<typename T::element_type>> // smart pointers
          : meta::lazy::if_<std::is_object<typename T::element_type>, typename T::element_type>
        {
            // The meta::lazy::if_ is because shared_ptr<void> is not Readable.
        };

        template<typename T>
        struct value_type<T, meta::if_<std::is_base_of<std::ios_base, T>>>
        {
            using type = typename T::char_type;
        };
        /// @}
    }
}

#endif
