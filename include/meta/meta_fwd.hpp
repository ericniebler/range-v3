/// \file meta_fwd.hpp Forward declarations
//
// Meta library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_FWD_HPP
#define META_FWD_HPP

#include <type_traits>
#include <utility>

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-variable-declarations"
#endif

#define META_CXX_STD_14 201402L
#define META_CXX_STD_17 201703L

#if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus // Older clangs define _MSVC_LANG < __cplusplus
#define META_CXX_VER _MSVC_LANG
#else
#define META_CXX_VER __cplusplus
#endif

#if defined(__apple_build_version__) || defined(__clang__)
#if defined(__apple_build_version__) || (defined(__clang__) && __clang_major__ < 6)
#define META_WORKAROUND_LLVM_28385 // https://llvm.org/bugs/show_bug.cgi?id=28385
#endif

#elif defined(_MSC_VER)
#define META_HAS_MAKE_INTEGER_SEQ 1
#define META_WORKAROUND_MSVC_702792 // Fixed for VS2019, possibly 16.0?
#define META_WORKAROUND_MSVC_703656 // Fixed for VS2019 16.0
#define META_WORKAROUND_MSVC_756112 // fold expression + alias templates in template argument

#elif defined(__GNUC__)
#define META_WORKAROUND_GCC_86356 // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86356
#if __GNUC__ < 8
#define META_WORKAROUND_GCC_UNKNOWN1 // Older GCCs don't like fold + debug + -march=native
#endif
#if __GNUC__ == 5 && __GNUC_MINOR__ == 1
#define META_WORKAROUND_GCC_66405 // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66405
#endif
#if __GNUC__ < 5
#define META_WORKAROUND_CWG_1558 // https://wg21.link/cwg1558
#endif
#endif

#ifndef META_CXX_VARIABLE_TEMPLATES
#ifdef __cpp_variable_templates
#define META_CXX_VARIABLE_TEMPLATES __cpp_variable_templates
#else
#define META_CXX_VARIABLE_TEMPLATES (META_CXX_VER >= META_CXX_STD_14)
#endif
#endif

#ifndef META_CXX_INLINE_VARIABLES
#ifdef __cpp_inline_variables
#define META_CXX_INLINE_VARIABLES __cpp_inline_variables
#else
#define META_CXX_INLINE_VARIABLES (META_CXX_VER >= META_CXX_STD_17)
#endif
#endif

#ifndef META_INLINE_VAR
#if META_CXX_INLINE_VARIABLES
#define META_INLINE_VAR inline
#else
#define META_INLINE_VAR
#endif
#endif

#ifndef META_CXX_INTEGER_SEQUENCE
#ifdef __cpp_lib_integer_sequence
#define META_CXX_INTEGER_SEQUENCE __cpp_lib_integer_sequence
#else
#define META_CXX_INTEGER_SEQUENCE (META_CXX_VER >= META_CXX_STD_14)
#endif
#endif

#ifndef META_HAS_MAKE_INTEGER_SEQ
#ifdef __has_builtin
#if __has_builtin(__make_integer_seq)
#define META_HAS_MAKE_INTEGER_SEQ 1
#endif
#endif
#endif
#ifndef META_HAS_MAKE_INTEGER_SEQ
#define META_HAS_MAKE_INTEGER_SEQ 0
#endif

#ifndef META_HAS_TYPE_PACK_ELEMENT
#ifdef __has_builtin
#if __has_builtin(__type_pack_element)
#define META_HAS_TYPE_PACK_ELEMENT 1
#endif
#endif
#endif
#ifndef META_HAS_TYPE_PACK_ELEMENT
#define META_HAS_TYPE_PACK_ELEMENT 0
#endif

#if !defined(META_DEPRECATED) && !defined(META_DISABLE_DEPRECATED_WARNINGS)
#if defined(__cpp_attribute_deprecated) || META_CXX_VER >= META_CXX_STD_14
#define META_DEPRECATED(...) [[deprecated(__VA_ARGS__)]]
#elif defined(__clang__) || defined(__GNUC__)
#define META_DEPRECATED(...) __attribute__((deprecated(__VA_ARGS__)))
#endif
#endif
#ifndef META_DEPRECATED
#define META_DEPRECATED(...)
#endif

#ifndef META_CXX_FOLD_EXPRESSIONS
#ifdef __cpp_fold_expressions
#define META_CXX_FOLD_EXPRESSIONS __cpp_fold_expressions
#else
#define META_CXX_FOLD_EXPRESSIONS (META_CXX_VER >= META_CXX_STD_17)
#endif
#endif

#if META_CXX_FOLD_EXPRESSIONS
#if !META_CXX_VARIABLE_TEMPLATES
#error Fold expressions, but no variable templates?
#endif
#endif

#if defined(__cpp_concepts) && __cpp_concepts > 0
#if !META_CXX_VARIABLE_TEMPLATES
#error Concepts, but no variable templates?
#endif
#if __cpp_concepts <= 201507L
#define META_CONCEPT concept bool
// TS concepts subsumption barrier for atomic expressions
#define META_CONCEPT_BARRIER(...) ::meta::detail::bool_<__VA_ARGS__>
#else
#define META_CONCEPT concept
#define META_CONCEPT_BARRIER(...) __VA_ARGS__
#endif
#define META_TYPE_CONSTRAINT(...) __VA_ARGS__
#else
#define META_TYPE_CONSTRAINT(...) typename
#endif

namespace meta
{
#if META_CXX_INTEGER_SEQUENCE
    using std::integer_sequence;
#else
    template <typename T, T...>
    struct integer_sequence;
#endif

    template <typename... Ts>
    struct list;

    template <typename T>
    struct id;

    template <template <typename...> class>
    struct quote;

    template <typename T, template <T...> class F>
    struct quote_i;

    template <template <typename...> class C, typename... Ts>
    struct defer;

    template <typename T, template <T...> class C, T... Is>
    struct defer_i;

#if META_CXX_VARIABLE_TEMPLATES || defined(META_DOXYGEN_INVOKED)
    /// is_v
    /// Test whether a type \p T is an instantiation of class
    /// template \p C.
    /// \ingroup trait
    template <typename, template <typename...> class>
    META_INLINE_VAR constexpr bool is_v = false;
    template <typename... Ts, template <typename...> class C>
    META_INLINE_VAR constexpr bool is_v<C<Ts...>, C> = true;
#endif

#ifdef META_CONCEPT
    namespace detail
    {
        template <bool B>
        META_INLINE_VAR constexpr bool bool_ = B;

        template <auto> struct require_constant; // not defined
    }

    template <typename...>
    META_CONCEPT True = META_CONCEPT_BARRIER(true);

    template <typename T, typename U>
    META_CONCEPT Same =
#if defined(__clang__)
        META_CONCEPT_BARRIER(__is_same(T, U));
#elif defined(__GNUC__) && __GNUC__ >= 6
        META_CONCEPT_BARRIER(__is_same_as(T, U));
#else
        META_CONCEPT_BARRIER(std::is_same_v<T, U>);
#endif

    template <template <typename...> class C, typename... Ts>
    META_CONCEPT Valid = requires
    {
        typename C<Ts...>;
    };

    template <typename T, template <T...> class C, T... Is>
    META_CONCEPT Valid_I = requires
    {
        typename C<Is...>;
    };

    template <typename T>
    META_CONCEPT Trait = requires
    {
        typename T::type;
    };

    template <typename T>
    META_CONCEPT Invocable = requires
    {
        typename quote<T::template invoke>;
    };

    template <typename T>
    META_CONCEPT List = is_v<T, list>;

    // clang-format off
    template <typename T>
    META_CONCEPT Integral = requires
    {
        typename T::type;
        typename T::value_type;
        typename T::type::value_type;
    }
    && Same<typename T::value_type, typename T::type::value_type>
    && std::is_integral_v<typename T::value_type>
    && requires
    {
        // { T::value } -> Same<const typename T::value_type&>;
        T::value;
        requires Same<decltype(T::value), const typename T::value_type>;
        typename detail::require_constant<T::value>;

        // { T::type::value } -> Same<const typename T::value_type&>;
        T::type::value;
        requires Same<decltype(T::type::value), const typename T::value_type>;
        typename detail::require_constant<T::type::value>;
        requires T::value == T::type::value;

        // { T{}() } -> Same<typename T::value_type>;
        T{}();
        requires Same<decltype(T{}()), typename T::value_type>;
        typename detail::require_constant<T{}()>;
        requires T{}() == T::value;

        { T{} } -> typename T::value_type;
    };
    // clang-format on
#endif // META_CONCEPT

    namespace extension
    {
        template <META_TYPE_CONSTRAINT(Invocable) F, typename L>
        struct apply;
    }
} // namespace meta

#ifdef __clang__
#pragma GCC diagnostic pop
#endif

#endif
