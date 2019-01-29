/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ITERATOR_ASSOCIATED_TYPES_HPP
#define RANGES_V3_ITERATOR_ASSOCIATED_TYPES_HPP

#include <cstddef>
#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <std/detail/associated_types.hpp>

namespace ranges
{
    /// \addtogroup group-concepts
    /// @{
    ////////////////////////////////////////////////////////////////////////////////////////
    /// \cond
    namespace detail
    {
#if defined(__GLIBCXX__)
        template<typename I>
        char (&is_std_iterator_traits_specialized_impl_(std::__iterator_traits<I> *))[2];
        template<typename I>
        char is_std_iterator_traits_specialized_impl_(void *);
#elif defined(_LIBCPP_VERSION)
        template<typename I, bool B>
        char (&is_std_iterator_traits_specialized_impl_(std::__iterator_traits<I, B> *))[2];
        template<typename I>
        char is_std_iterator_traits_specialized_impl_(void *);
#elif defined(_MSVC_STL_VERSION)
        template<typename I>
        char (&is_std_iterator_traits_specialized_impl_(std::_Iterator_traits_base<I> *))[2];
        template<typename I>
        char is_std_iterator_traits_specialized_impl_(void *);
#else
        template<typename I>
        char (&is_std_iterator_traits_specialized_impl_(void *))[2];
#endif
        template<typename, typename T>
        char (&is_std_iterator_traits_specialized_impl_(std::iterator_traits<T *> *))[2];

        template<typename I>
        constexpr bool is_std_iterator_traits_specialized_ =
            1 == sizeof(is_std_iterator_traits_specialized_impl_<I>(
                static_cast<std::iterator_traits<I> *>(nullptr)));

        // The standard iterator_traits<T *> specialization(s) do not count
        // as user-specialized. This will no longer be necessary in C++20.
        // This helps with `T volatile*` and `void *`.
        template<typename T>
        constexpr bool is_std_iterator_traits_specialized_<T *> = false;
    }
    /// \endcond

#if defined(RANGES_DEEP_STL_INTEGRATION) && RANGES_DEEP_STL_INTEGRATION && \
    !defined(RANGES_DOXYGEN_INVOKED)
    template<typename T>
    using iter_difference_t =
        typename detail::if_then_t<
            detail::is_std_iterator_traits_specialized_<T>,
            std::iterator_traits<T>,
            incrementable_traits<T>>::difference_type;
#else
    template<typename T>
    using iter_difference_t =
        typename incrementable_traits<T>::difference_type;
#endif

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
#ifdef RANGES_WORKAROUND_MSVC_683388
            typename R = meta::if_<
                meta::and_<std::is_pointer<uncvref_t<I>>,
                    std::is_array<std::remove_pointer_t<uncvref_t<I>>>>,
                std::add_lvalue_reference_t<std::remove_pointer_t<uncvref_t<I>>>,
                decltype(*std::declval<I &>())>,
#else
            typename R = decltype(*std::declval<I &>()),
#endif
            typename = R&>
        using iter_reference_t_ = R;

        template<typename, typename = void>
        struct size_type_
        {};

        template<typename T>
        struct size_type_<T, always_<void, iter_difference_t<T>>>
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

#if defined(RANGES_DEEP_STL_INTEGRATION) && RANGES_DEEP_STL_INTEGRATION && \
    !defined(RANGES_DOXYGEN_INVOKED)
    template<typename T>
    using iter_value_t =
        typename detail::if_then_t<
            detail::is_std_iterator_traits_specialized_<T>,
            std::iterator_traits<T>,
            readable_traits<T>>::value_type;
#else
    template<typename T>
    using iter_value_t =
        typename readable_traits<T>::value_type;
#endif

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

    // Defined in range_fwd.hpp:
    // template<typename S, typename I>
    // inline constexpr bool disable_sized_sentinel = false;

    /// @}
}

#endif
