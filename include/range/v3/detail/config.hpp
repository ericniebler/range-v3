/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DETAIL_CONFIG_HPP
#define RANGES_V3_DETAIL_CONFIG_HPP

#include <iosfwd>

#ifndef RANGES_ASSERT
#include <cassert>
#define RANGES_ASSERT assert
#endif

#ifndef RANGES_ENSURE_MSG
#include <exception>
#define RANGES_ENSURE_MSG(COND, MSG) \
    ((COND) ? void() : (RANGES_ASSERT(!(true && MSG)), std::terminate()))
#endif

#ifndef RANGES_ENSURE
#define RANGES_ENSURE(COND) \
    RANGES_ENSURE_MSG(COND, #COND)
#endif

#define RANGES_DECLTYPE_AUTO_RETURN(...)                        \
    -> decltype(__VA_ARGS__)                                    \
    { return (__VA_ARGS__); }                                   \
    /**/

#define RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(...)               \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__))) ->   \
    decltype(__VA_ARGS__)                                       \
    { return (__VA_ARGS__); }                                   \
    /**/

#define RANGES_DECLTYPE_NOEXCEPT(...)                           \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__))) ->   \
    decltype(__VA_ARGS__)                                       \
    /**/

// Non-portable forward declarations of standard containers
#ifdef _LIBCPP_VERSION
#define RANGES_BEGIN_NAMESPACE_STD _LIBCPP_BEGIN_NAMESPACE_STD
#define RANGES_END_NAMESPACE_STD _LIBCPP_END_NAMESPACE_STD
#else
#define RANGES_BEGIN_NAMESPACE_STD namespace std {
#define RANGES_END_NAMESPACE_STD }
#endif

// Database of feature versions
#define RANGES_CXX_STATIC_ASSERT_11 200410
#define RANGES_CXX_STATIC_ASSERT_14 RANGES_CXX_STATIC_ASSERT_11
#define RANGES_CXX_STATIC_ASSERT_17 201411
#define RANGES_CXX_VARIABLE_TEMPLATES_11 0
#define RANGES_CXX_VARIABLE_TEMPLATES_14 201304
#define RANGES_CXX_ATTRIBUTE_DEPRECATED_11 0
#define RANGES_CXX_ATTRIBUTE_DEPRECATED_14 201309
#define RANGES_CXX_CONSTEXPR_11 200704
#define RANGES_CXX_CONSTEXPR_14 201304
#define RANGES_CXX_RANGE_BASED_FOR_11 200907
#define RANGES_CXX_RANGE_BASED_FOR_14 RANGES_CXX_RANGE_BASED_FOR_11
#define RANGES_CXX_RANGE_BASED_FOR_17 201603
#define RANGES_CXX_LIB_IS_FINAL_11 0
#define RANGES_CXX_LIB_IS_FINAL_14 201402
#define RANGES_CXX_RETURN_TYPE_DEDUCTION_11 0
#define RANGES_CXX_RETURN_TYPE_DEDUCTION_14 201304
#define RANGES_CXX_GENERIC_LAMBDAS_11 0
#define RANGES_CXX_GENERIC_LAMBDAS_14 201304
#define RANGES_CXX_STD_11 201103
#define RANGES_CXX_STD_14 201402
#define RANGES_CXX_THREAD_LOCAL_PRE_STANDARD 200000 // Arbrarily chosen number between 0 and C++11
#define RANGES_CXX_THREAD_LOCAL_11 RANGES_CXX_STD_11
#define RANGES_CXX_THREAD_LOCAL_14 RANGES_CXX_THREAD_LOCAL_11

#if defined(_MSC_VER) && !defined(__clang__)
#if _MSC_VER >= 1900
#ifndef RANGES_CXX_VARIABLE_TEMPLATES
#define RANGES_CXX_VARIABLE_TEMPLATES RANGES_CXX_VARIABLE_TEMPLATES_14
#endif
#ifndef RANGES_CXX_ATTRIBUTE_DEPRECATED
#define RANGES_CXX_ATTRIBUTE_DEPRECATED RANGES_CXX_ATTRIBUTE_DEPRECATED_14
#endif
#if !defined(RANGES_CXX_RANGE_BASED_FOR) && defined(_MSVC_LANG) && _MSVC_LANG > 201402
#define RANGES_CXX_RANGE_BASED_FOR RANGES_CXX_RANGE_BASED_FOR_17
#endif
#ifndef RANGES_CXX_LIB_IS_FINAL
#define RANGES_CXX_LIB_IS_FINAL RANGES_CXX_LIB_IS_FINAL_14
#endif
#ifndef RANGES_CXX_RETURN_TYPE_DEDUCTION
#define RANGES_CXX_RETURN_TYPE_DEDUCTION RANGES_CXX_RETURN_TYPE_DEDUCTION_14
#endif
#ifndef RANGES_CXX_GENERIC_LAMBDAS
#define RANGES_CXX_GENERIC_LAMBDAS RANGES_CXX_GENERIC_LAMBDAS_14
#endif

#else // _MSC_VER < 1900
#error Unsupported version of Visual C++
#endif // _MSC_VER switch

#define RANGES_DIAGNOSTIC_PUSH __pragma(warning(push))
#define RANGES_DIAGNOSTIC_POP __pragma(warning(pop))
#define RANGES_DIAGNOSTIC_IGNORE(X) __pragma(warning(disable:X))
#define RANGES_DIAGNOSTIC_IGNORE_SHADOWING RANGES_DIAGNOSTIC_IGNORE(4456)
#define RANGES_DIAGNOSTIC_IGNORE_PRAGMAS RANGES_DIAGNOSTIC_IGNORE(4068)
#define RANGES_DIAGNOSTIC_IGNORE_INDENTATION
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
#define RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL

#else // ^^^ defined(_MSC_VER) ^^^ / vvv !defined(_MSC_VER) vvv
// Generic configuration using SD-6 feature test macros with fallback to __cplusplus
#if defined(__GNUC__) || defined(__clang__)
#define RANGES_PRAGMA(X) _Pragma(#X)
#define RANGES_DIAGNOSTIC_PUSH RANGES_PRAGMA(GCC diagnostic push)
#define RANGES_DIAGNOSTIC_POP RANGES_PRAGMA(GCC diagnostic pop)
#define RANGES_DIAGNOSTIC_IGNORE(X) RANGES_PRAGMA(GCC diagnostic ignored X)
#define RANGES_DIAGNOSTIC_IGNORE_SHADOWING RANGES_DIAGNOSTIC_IGNORE("-Wshadow")
#define RANGES_DIAGNOSTIC_IGNORE_PRAGMAS RANGES_DIAGNOSTIC_IGNORE("-Wunknown-pragmas") RANGES_DIAGNOSTIC_IGNORE("-Wpragmas")
#define RANGES_DIAGNOSTIC_IGNORE_INDENTATION RANGES_DIAGNOSTIC_IGNORE("-Wmisleading-indentation")
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL RANGES_DIAGNOSTIC_IGNORE("-Wundefined-internal")
#define RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS RANGES_DIAGNOSTIC_IGNORE("-Wmismatched-tags")
#define RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION RANGES_DIAGNOSTIC_IGNORE("-Wsign-conversion")
#ifdef __clang__
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS RANGES_DIAGNOSTIC_IGNORE("-Wglobal-constructors")
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL RANGES_DIAGNOSTIC_IGNORE("-Wunneeded-internal-declaration")
#else
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL
#endif
#else
#define RANGES_DIAGNOSTIC_PUSH
#define RANGES_DIAGNOSTIC_POP
#define RANGES_DIAGNOSTIC_IGNORE_SHADOWING
#define RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
#define RANGES_DIAGNOSTIC_IGNORE_INDENTATION
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
#define RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL
#endif
#endif // MSVC/Generic configuration switch

#define RANGES_CXX_FEATURE_CONCAT2(y, z) RANGES_CXX_ ## y ## _ ## z
#define RANGES_CXX_FEATURE_CONCAT(y, z) RANGES_CXX_FEATURE_CONCAT2(y, z)
#define RANGES_CXX_FEATURE(x) RANGES_CXX_FEATURE_CONCAT(x, RANGES_CXX_STD_NAME)

#if __cplusplus >= RANGES_CXX_STD_14
#define RANGES_CXX_STD_NAME 14
#define RANGES_CXX_STD RANGES_CXX_STD_14
#else
#define RANGES_CXX_STD_NAME 11
#define RANGES_CXX_STD RANGES_CXX_STD_11
#endif

#ifndef RANGES_CXX_STATIC_ASSERT
#ifdef __cpp_static_assert
#define RANGES_CXX_STATIC_ASSERT __cpp_static_assert
#else
#define RANGES_CXX_STATIC_ASSERT RANGES_CXX_FEATURE(STATIC_ASSERT)
#endif
#endif

#ifndef RANGES_CXX_VARIABLE_TEMPLATES
#ifdef __cpp_variable_templates
#define RANGES_CXX_VARIABLE_TEMPLATES __cpp_variable_templates
#else
#define RANGES_CXX_VARIABLE_TEMPLATES RANGES_CXX_FEATURE(VARIABLE_TEMPLATES)
#endif
#endif

#ifndef RANGES_CXX_ATTRIBUTE_DEPRECATED
#ifdef __has_cpp_attribute
#define RANGES_CXX_ATTRIBUTE_DEPRECATED __has_cpp_attribute(deprecated)
#elif defined(__cpp_attribute_deprecated)
#define RANGES_CXX_ATTRIBUTE_DEPRECATED __cpp_attribute_deprecated
#else
#define RANGES_CXX_ATTRIBUTE_DEPRECATED RANGES_CXX_FEATURE(ATTRIBUTE_DEPRECATED)
#endif
#endif

#ifndef RANGES_CXX_CONSTEXPR
#ifdef __cpp_constexpr
#define RANGES_CXX_CONSTEXPR __cpp_constexpr
#else
#define RANGES_CXX_CONSTEXPR RANGES_CXX_FEATURE(CONSTEXPR)
#endif
#endif

#ifndef RANGES_CXX_RANGE_BASED_FOR
#ifdef __cpp_range_based_for
#define RANGES_CXX_RANGE_BASED_FOR __cpp_range_based_for
#else
#define RANGES_CXX_RANGE_BASED_FOR RANGES_CXX_FEATURE(RANGE_BASED_FOR)
#endif
#endif

#ifndef RANGES_CXX_LIB_IS_FINAL
#include <type_traits>
#ifdef __cpp_lib_is_final
#define RANGES_CXX_LIB_IS_FINAL __cpp_lib_is_final
#else
#define RANGES_CXX_LIB_IS_FINAL RANGES_CXX_FEATURE(LIB_IS_FINAL)
#endif
#endif

#ifndef RANGES_CXX_RETURN_TYPE_DEDUCTION
#ifdef __cpp_return_type_deduction
#define RANGES_CXX_RETURN_TYPE_DEDUCTION __cpp_return_type_deduction
#else
#define RANGES_CXX_RETURN_TYPE_DEDUCTION RANGES_CXX_FEATURE(RETURN_TYPE_DEDUCTION)
#endif
#endif

#ifndef RANGES_CXX_GENERIC_LAMBDAS
#ifdef __cpp_generic_lambdas
#define RANGES_CXX_GENERIC_LAMBDAS __cpp_generic_lambdas
#else
#define RANGES_CXX_GENERIC_LAMBDAS RANGES_CXX_FEATURE(GENERIC_LAMBDAS)
#endif
#endif

#ifndef RANGES_CXX_THREAD_LOCAL
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED <= 70100
#define RANGES_CXX_THREAD_LOCAL 0
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED) || \
    (defined(__clang__) && (defined(__CYGWIN__) || defined(__apple_build_version__)))
// BUGBUG avoid unresolved __cxa_thread_atexit
#define RANGES_CXX_THREAD_LOCAL RANGES_CXX_THREAD_LOCAL_PRE_STANDARD
#else
#define RANGES_CXX_THREAD_LOCAL RANGES_CXX_FEATURE(THREAD_LOCAL)
#endif
#endif

#ifndef RANGES_DISABLE_DEPRECATED_WARNINGS
#if RANGES_CXX_ATTRIBUTE_DEPRECATED && \
   !((defined(__clang__) || defined(__GNUC__)) && \
     RANGES_CXX_STD < RANGES_CXX_STD_14)
#define RANGES_DEPRECATED(MSG) [[deprecated(MSG)]]
#elif defined(__clang__) || defined(__GNUC__)
#define RANGES_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#elif defined(_MSC_VER)
#define RANGES_DEPRECATED(MSG) __declspec(deprecated(MSG))
#else
#define RANGES_DEPRECATED(MSG)
#endif
#else
#define RANGES_DEPRECATED(MSG)
#endif

// RANGES_CXX14_CONSTEXPR macro (see also BOOST_CXX14_CONSTEXPR)
// Note: constexpr implies inline, to retain the same visibility
// C++14 constexpr functions are inline in C++11
#if RANGES_CXX_CONSTEXPR >= RANGES_CXX_CONSTEXPR_14
#define RANGES_CXX14_CONSTEXPR constexpr
#else
#define RANGES_CXX14_CONSTEXPR inline
#endif

#ifdef NDEBUG
#define RANGES_NDEBUG_CONSTEXPR constexpr
#else
#define RANGES_NDEBUG_CONSTEXPR inline
#endif

#define RANGES_CXX_INLINE_VARIABLES_11 0
#define RANGES_CXX_INLINE_VARIABLES_14 0
#define RANGES_CXX_INLINE_VARIABLES_17 201606
#ifndef RANGES_CXX_INLINE_VARIABLES

#ifdef __cpp_inline_variables // TODO: fix this if SD-6 picks another name
#define RANGES_CXX_INLINE_VARIABLES __cpp_inline_variables
#elif defined(__clang__) && \
    (__clang__major__ > 3 || __clang_major__ == 3 && __clang_minor__ == 9) && \
    __cplusplus > 201402L
// TODO: remove once clang defines __cpp_inline_variables (or equivalent)
#define RANGES_CXX_INLINE_VARIABLES RANGES_CXX_INLINE_VARIABLES_17
#else
#define RANGES_CXX_INLINE_VARIABLES RANGES_CXX_FEATURE(INLINE_VARIABLES)
#endif  // __cpp_inline_variables

#endif  // RANGES_CXX_INLINE_VARIABLES

#if RANGES_CXX_INLINE_VARIABLES < RANGES_CXX_INLINE_VARIABLES_17
#define RANGES_INLINE_VARIABLE(type, name)                  \
    inline namespace                   \
    {                                                       \
        constexpr auto& name = static_const<type>::value;   \
    }                                                     

#else  // RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
#define RANGES_INLINE_VARIABLE(type, name) \
    inline namespace function_objects             \
    {                                      \
        inline constexpr type name{};      \
    }
#endif // RANGES_CXX_INLINE_VARIABLES

#ifdef RANGES_FEWER_WARNINGS
#define RANGES_DISABLE_WARNINGS                 \
    RANGES_DIAGNOSTIC_PUSH                      \
    RANGES_DIAGNOSTIC_IGNORE_PRAGMAS            \
    RANGES_DIAGNOSTIC_IGNORE_SHADOWING          \
    RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL \
    RANGES_DIAGNOSTIC_IGNORE_INDENTATION

#define RANGES_RE_ENABLE_WARNINGS RANGES_DIAGNOSTIC_POP
#else
#define RANGES_DISABLE_WARNINGS
#define RANGES_RE_ENABLE_WARNINGS
#endif

namespace ranges {
    inline namespace v3 {
        namespace detail {
            namespace ebo_test {
                struct empty1 {};
                struct empty2 {};
                struct empty3 {};
                struct refines : empty1, empty2, empty3 {};
            }
            constexpr bool broken_ebo = sizeof(ebo_test::refines) > sizeof(ebo_test::empty1);
        }
    }
}

#endif
