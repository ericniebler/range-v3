/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014, 2016
//  Copyright Casey Carter 2016
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
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/nullptr_v.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        /// @{

        ////////////////////////////////////////////////////////////////////////////////////////
        /// \cond
        namespace detail
        {
            template<typename T, typename Enable = void>
            struct difference_type2
            {};

            template<typename T>
            struct difference_type2<T,
                meta::if_<std::is_integral<
                    decltype(std::declval<const T>() - std::declval<const T>())>>>
              : std::make_signed<decltype(std::declval<const T>() - std::declval<const T>())>
            {};

            template<typename T, typename Enable = void>
            struct difference_type1
            : detail::difference_type2<T>
            {};

            template<typename T>
            struct difference_type1<T *>
            : meta::lazy::if_<std::is_object<T>, std::ptrdiff_t>
            {};

            template<typename T>
            struct difference_type1<T, meta::void_<typename T::difference_type>>
            {
                using type = typename T::difference_type;
            };
        }
        /// \endcond

        template<typename T>
        struct difference_type
        : detail::difference_type1<T>
        {};

        template<typename T>
        struct difference_type<T const>
        : difference_type<T>
        {};

        ////////////////////////////////////////////////////////////////////////////////////////
        /// \cond
        namespace detail
        {
            template<typename I,
                typename R = decltype(*std::declval<I &>()),
                typename = R&>
            using reference_t_ = R;
        }
        /// \endcond

        template<typename R>
        using reference_t = detail::reference_t_<R>;

        ////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        struct size_type
          : meta::lazy::let<std::make_unsigned<meta::lazy::_t<difference_type<T>>>>
        {};

        /// \cond
        namespace detail
        {
#if !defined(__GNUC__) || defined(__clang__)
            // GCC does not implement CWG393
            // per https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69316
            template<typename T>
            std::remove_cv<T> value_type_helper(T (*)[]);
#endif
            template<typename T, std::size_t N>
            std::remove_cv<T> value_type_helper(T (*)[N]);

            template<typename T>
            using object_remove_cv = meta::if_<std::is_object<T>, std::remove_cv<T>>;

            template<typename T>
            object_remove_cv<T> value_type_helper(T **);

            template<typename T>
            object_remove_cv<typename T::value_type> value_type_helper(T *);

            template<typename T>
            object_remove_cv<typename T::element_type> value_type_helper(T *);

            template<typename T>
            meta::if_<std::is_base_of<std::ios_base, T>, std::remove_cv<typename T::char_type>>
            value_type_helper(T *);

            template<typename T>
            using value_type_ = meta::_t<decltype(detail::value_type_helper(_nullptr_v<T>()))>;
        }
        /// \endcond

        ////////////////////////////////////////////////////////////////////////////////////////
        // Not to spec:
        // * arrays of unknown bound have no value type on compilers that do not implement
        //   CWG 393 (http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#393).
        // * For class types with member type element_type, value type is element_type with any
        //   cv-qualifiers stripped (See ericniebler/stl2#299).
        // * using member "char_type" as the value type of class types derived from
        //   std::ios_base is an extension.
        template<typename T>
        struct value_type
          : meta::defer<detail::value_type_, T>
        {};

        template<typename T>
        struct value_type<T const>
          : value_type<T>
        {};

        template<typename S, typename I>
        struct disable_sized_sentinel : std::false_type {};
        /// @}
    }
}

#endif
