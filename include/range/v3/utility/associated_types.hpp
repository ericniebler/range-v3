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
#include <iterator>
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
#if defined(__GLIBCXX__)
            template<typename I>
            std::false_type is_std_iterator_traits_specialized_(std::__iterator_traits<I> *);
            template<typename I>
            std::true_type is_std_iterator_traits_specialized_(void *);
#elif defined(_LIBCPP_VERSION)
            template<typename I, bool B>
            std::false_type is_std_iterator_traits_specialized_(std::__iterator_traits<I, B> *);
            template<typename I>
            std::true_type is_std_iterator_traits_specialized_(void *);
#elif defined(_MSVC_STL_VERSION)
            template<typename I>
            std::false_type is_std_iterator_traits_specialized_(std::_Iterator_traits_base<I /*, void*/> *);
            template<typename I>
            std::true_type is_std_iterator_traits_specialized_(void *);
#else
            template<typename I>
            std::false_type is_std_iterator_traits_specialized_(void *);
#endif

            template<typename I>
            struct is_std_iterator_traits_specialized
              : decltype(is_std_iterator_traits_specialized_<I>(
                    static_cast<std::iterator_traits<I> *>(nullptr)))
            {};

            template<typename T>
            struct with_difference_type_
            {
                using difference_type = T;
            };

            template<typename T>
            using difference_result_t =
                decltype(std::declval<const T>() - std::declval<const T>());

            template<typename, typename = void>
            struct incrementable_traits_2_
            {};

            template<typename T>
            struct incrementable_traits_2_<T, meta::if_<std::is_integral<difference_result_t<T>>>>
            {
                using difference_type = meta::_t<std::make_signed<difference_result_t<T>>>;
            };

            template<typename T, typename = void>
            struct incrementable_traits_1_
              : detail::incrementable_traits_2_<T>
            {};

            template<typename T>
            struct incrementable_traits_1_<T *>
              : meta::if_<
                    std::is_object<T>,
                    detail::with_difference_type_<std::ptrdiff_t>,
                    meta::nil_>
            {};

            template<typename T>
            struct incrementable_traits_1_<T, meta::void_<typename T::difference_type>>
            {
                using difference_type = typename T::difference_type;
            };
        }
        /// \endcond

        template<typename T>
        struct incrementable_traits
          : detail::incrementable_traits_1_<T>
        {};

        template<typename T>
        struct incrementable_traits<T const>
          : incrementable_traits<T>
        {};

        template<typename T>
        using iter_difference_t =
            typename meta::if_<
                detail::is_std_iterator_traits_specialized<T>,
                std::iterator_traits<T>,
                incrementable_traits<T>>::difference_type;

        ////////////////////////////////////////////////////////////////////////////////////////
        /// \cond
        template<typename T>
        struct difference_type_
          : meta::defer<iter_difference_t, T>
        {};

        template<typename T>
        using difference_type_t
            RANGES_DEPRECATED("ranges::difference_type_t is deprecated. Please use "
                "ranges::iter_difference_t instead.") =
            iter_difference_t<T>;
        /// \endcond

        /// \cond
        namespace detail
        {
            template<typename I,
                typename R = decltype(*std::declval<I &>()),
                typename = R&>
            using iter_reference_t_ = R;

            template<typename, typename = void>
            struct size_type_
            {};

            template<typename T>
            struct size_type_<T, meta::void_<iter_difference_t<T>>>
              : std::make_unsigned<iter_difference_t<T>>
            {};
        }
        /// \endcond

        template<typename R>
        using iter_reference_t = detail::iter_reference_t_<R>;

        template<typename R>
        using reference_t
            RANGES_DEPRECATED("ranges::reference_t is deprecated. Use ranges::iter_reference_t "
                "instead.") =
            iter_reference_t<R>;

        ////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        struct size_type
          : detail::size_type_<T>
        {};

        /// \cond
        namespace detail
        {
            template<typename, typename = void>
            struct with_value_type_
            {};
            template<typename T>
            struct with_value_type_<T, meta::if_<std::is_object<T>>>
            {
                using value_type = meta::_t<std::remove_cv<T>>;
            };
            template<typename, typename = void>
            struct readable_traits_2_
            {};
            template<typename T>
            struct readable_traits_2_<T, meta::void_<typename T::element_type>>
              : with_value_type_<typename T::element_type>
            {};
            template<typename T, typename = void>
            struct readable_traits_1_
              : readable_traits_2_<T>
            {};
            template<typename T>
            struct readable_traits_1_<T, meta::if_<std::is_array<T>>>
              : with_value_type_<meta::_t<std::remove_extent<T>>>
            {};
            template<typename T>
            struct readable_traits_1_<T *>
              : detail::with_value_type_<T>
            {};
            template<typename T>
            struct readable_traits_1_<T, meta::void_<typename T::value_type>>
              : with_value_type_<typename T::value_type>
            {};
        }
        /// \endcond

        ////////////////////////////////////////////////////////////////////////////////////////
        // Not to spec:
        // * For class types with both member value_type and element_type, value_type is
        //   preferred (see ericniebler/stl2#299).
        template<typename T>
        struct readable_traits
          : detail::readable_traits_1_<T>
        {};

        template<typename T>
        struct readable_traits<T const>
          : readable_traits<T>
        {};

        template<typename T>
        using iter_value_t =
            typename meta::if_<
                detail::is_std_iterator_traits_specialized<T>,
                std::iterator_traits<T>,
                readable_traits<T>>::value_type;

        /// \cond
        template<typename T>
        struct value_type_
          : meta::defer<iter_value_t, T>
        {};

        template<typename T>
        using value_type_t
            RANGES_DEPRECATED("ranges::value_type_t is deprecated. Please use "
                "ranges::iter_value_t instead.") =
            iter_value_t<T>;
        /// \endcond

        template<typename S, typename I>
        struct disable_sized_sentinel
          : std::false_type
        {};
        /// @}
    }
}

#endif
