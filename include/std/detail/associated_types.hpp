// Range v3 library
//
//  Copyright Eric Niebler 2013-2014, 2016-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_STD_DETAIL_ASSOCIATED_TYPES_HPP
#define RANGES_V3_STD_DETAIL_ASSOCIATED_TYPES_HPP

#include <climits>
#include <cstdint>
#include <range/v3/detail/config.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{

    ////////////////////////////////////////////////////////////////////////////////////////
    /// \cond
    namespace detail
    {
        struct nil_ {};

        template<typename T, typename...>
        using always_ = T;

        template<bool>
        struct if_then
        {
            template<typename, typename U>
            using apply = U;
        };
        template<>
        struct if_then<true>
        {
            template<typename T, typename>
            using apply = T;
        };
        template<bool B, typename T, typename U>
        using if_then_t = typename if_then<B>::template apply<T, U>;

        template<bool>
        struct enable_if
        {};
        template<>
        struct enable_if<true>
        {
            template<typename T>
            using apply = T;
        };
        template<bool B, typename T = void>
        using enable_if_t = typename enable_if<B>::template apply<T>;

        void is_objptr_(void const volatile *);
        #ifdef _MSC_VER
        // Microsoft's compiler permits function pointers to implicitly
        // convert to void*.
        template<class R, class... Args>
        void is_objptr_(R(*)(Args...)) = delete;
        #endif

        // std::is_object, optimized for compile time.
        template<typename T>
        constexpr bool is_object_(long)
        {
            return false;
        }
        template<typename T>
        constexpr bool is_object_(int,
            T *(*q)(T&) = nullptr,
            T *p = nullptr,
            decltype(detail::is_objptr_(q(*p)))* = nullptr)
        {
            return (void)p, (void)q, true;
        }

        template<typename T>
        constexpr bool is_integral_(...)
        {
            return false;
        }
        template<typename T, T = 1>
        constexpr bool is_integral_(long)
        {
            return true;
        }
        #if defined(__cpp_nontype_template_parameter_class) && \
            __cpp_nontype_template_parameter_class > 0
        template<typename T>
        constexpr bool is_integral_(int, int T::* = nullptr)
        {
            return false;
        }
        #endif

        template<typename T>
        struct with_difference_type_
        {
            using difference_type = T;
        };

        template<typename T>
        using difference_result_t =
            decltype(std::declval<T const &>() - std::declval<T const &>());

        template<typename, typename = void>
        struct incrementable_traits_2_
        {};

        template<typename T>
        struct incrementable_traits_2_<
            T,
#ifdef RANGES_WORKAROUND_MSVC_785522
            std::enable_if_t<std::is_integral_v<difference_result_t<T>>>>
#else // ^^^ workaround / no workaround vvv
            always_<void, int[is_integral_<difference_result_t<T>>(0)]>>
#endif // RANGES_WORKAROUND_MSVC_785522
        {
            using difference_type = std::make_signed_t<difference_result_t<T>>;
        };

        template<typename T, typename = void>
        struct incrementable_traits_1_
          : incrementable_traits_2_<T>
        {};

        template<typename T>
        struct incrementable_traits_1_<T *>
          : if_then_t<
                is_object_<T>(0),
                with_difference_type_<std::ptrdiff_t>,
                nil_>
        {};

        template<typename T>
        struct incrementable_traits_1_<T, always_<void, typename T::difference_type>>
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

    /// \cond
    namespace detail
    {
        template<typename T, bool = is_object_<T>(0)>
        struct with_value_type_
        {};
        template<typename T>
        struct with_value_type_<T, true>
        {
            using value_type = T;
        };
        template<typename T>
        struct with_value_type_<T const, true>
        {
            using value_type = T;
        };
        template<typename T>
        struct with_value_type_<T volatile, true>
        {
            using value_type = T;
        };
        template<typename T>
        struct with_value_type_<T const volatile, true>
        {
            using value_type = T;
        };
        template<typename, typename = void>
        struct readable_traits_2_
        {};
        template<typename T>
        struct readable_traits_2_<T, always_<void, typename T::element_type>>
          : with_value_type_<typename T::element_type>
        {};
        template<typename T, typename = void>
        struct readable_traits_1_
          : readable_traits_2_<T>
        {};
        template<typename T>
        struct readable_traits_1_<T[]>
          : with_value_type_<T>
        {};
        template<typename T, std::size_t N>
        struct readable_traits_1_<T[N]>
          : with_value_type_<T>
        {};
        template<typename T>
        struct readable_traits_1_<T *>
          : detail::with_value_type_<T>
        {};
        template<typename T>
        struct readable_traits_1_<T, always_<void, typename T::value_type>>
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

    /// \cond
    namespace detail
    {
        template<typename D = std::ptrdiff_t>
        struct std_output_iterator_traits
        {
            using iterator_category = std::output_iterator_tag;
            using difference_type = D;
            using value_type = void;
            using reference = void;
            using pointer = void;
        };

        // For testing whether a particular instantiation of std::iterator_traits
        // is user-specified or not.
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
        RANGES_INLINE_VAR constexpr bool is_std_iterator_traits_specialized_v =
            1 == sizeof(is_std_iterator_traits_specialized_impl_<I>(
                static_cast<std::iterator_traits<I> *>(nullptr)));

        // The standard iterator_traits<T *> specialization(s) do not count
        // as user-specialized. This will no longer be necessary in C++20.
        // This helps with `T volatile*` and `void *`.
        template<typename T>
        RANGES_INLINE_VAR constexpr bool is_std_iterator_traits_specialized_v<T *> = false;
    }
    /// \endcond
}

#endif
