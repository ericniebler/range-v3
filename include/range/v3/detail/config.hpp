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

#ifndef RANGES_V3_DETAIL_CONFIG_HPP
#define RANGES_V3_DETAIL_CONFIG_HPP

#include <iosfwd>
#if (defined(NDEBUG) && !defined(RANGES_ENSURE_MSG)) || \
    (!defined(NDEBUG) && !defined(RANGES_ASSERT) && \
     ((defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 5 || defined(__MINGW32__))) || \
      defined(_MSVC_STL_VERSION)))
#include <cstdio>
#include <cstdlib>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<class = void>
            [[noreturn]] void assert_failure(char const *file, int line, char const *msg)
            {
                std::fprintf(stderr, "%s(%d): %s\n", file, line, msg);
                std::abort();
            }
        }
    }
}
#endif

#ifndef RANGES_ASSERT
    // Always use our hand-rolled assert implementation on older GCCs, which do
    // not allow assert to be used in a constant expression, and on MSVC whose
    // assert is not marked [[noreturn]].
#if !defined(NDEBUG) && \
    ((defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 5 || defined(__MINGW32__))) || \
     defined(_MSVC_STL_VERSION))
#define RANGES_ASSERT(...) \
    static_cast<void>((__VA_ARGS__) ? void(0) : \
        ::ranges::detail::assert_failure(__FILE__, __LINE__, "assertion failed: " #__VA_ARGS__))
#else
#include <cassert>
#define RANGES_ASSERT assert
#endif
#endif

#ifndef RANGES_ASSUME
#if defined(__clang__) || defined(__GNUC__)
#define RANGES_ASSUME(COND) static_cast<void>((COND) ? void(0) : __builtin_unreachable())
#elif defined(_MSC_VER)
#define RANGES_ASSUME(COND) static_cast<void>(__assume(COND))
#else
#define RANGES_ASSUME(COND) static_cast<void>(COND)
#endif
#endif // RANGES_ASSUME

#ifndef RANGES_EXPECT
#ifdef NDEBUG
#define RANGES_EXPECT(COND) RANGES_ASSUME(COND)
#else // NDEBUG
#define RANGES_EXPECT(COND) RANGES_ASSERT(COND)
#endif // NDEBUG
#endif // RANGES_EXPECT

#ifndef RANGES_ENSURE_MSG
#if defined(NDEBUG)
#define RANGES_ENSURE_MSG(COND, MSG) \
    static_cast<void>((COND) ? void(0) \
        : ::ranges::detail::assert_failure(__FILE__, __LINE__, "ensure failed: " MSG))
#else
#define RANGES_ENSURE_MSG(COND, MSG) RANGES_ASSERT((COND) && MSG)
#endif
#endif

#ifndef RANGES_ENSURE
#define RANGES_ENSURE(...) RANGES_ENSURE_MSG((__VA_ARGS__), #__VA_ARGS__)
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

#define RANGES_AUTO_RETURN_NOEXCEPT(...)                        \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__)))      \
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
#elif defined(_MSVC_STL_VERSION)
#define RANGES_BEGIN_NAMESPACE_STD _STD_BEGIN
#define RANGES_END_NAMESPACE_STD _STD_END
#elif defined(_GLIBCXX_DEBUG)
#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
#define RANGES_NO_STD_FORWARD_DECLARATIONS
#endif
#else
#define RANGES_BEGIN_NAMESPACE_STD namespace std {
#define RANGES_END_NAMESPACE_STD }
#endif

// Database of feature versions
#define RANGES_CXX_STATIC_ASSERT_11 200410L
#define RANGES_CXX_STATIC_ASSERT_14 RANGES_CXX_STATIC_ASSERT_11
#define RANGES_CXX_STATIC_ASSERT_17 201411L
#define RANGES_CXX_VARIABLE_TEMPLATES_11 0L
#define RANGES_CXX_VARIABLE_TEMPLATES_14 201304L
#define RANGES_CXX_VARIABLE_TEMPLATES_17 RANGES_CXX_VARIABLE_TEMPLATES_14
#define RANGES_CXX_ATTRIBUTE_DEPRECATED_11 0L
#define RANGES_CXX_ATTRIBUTE_DEPRECATED_14 201309L
#define RANGES_CXX_ATTRIBUTE_DEPRECATED_17 RANGES_CXX_ATTRIBUTE_DEPRECATED_14
#define RANGES_CXX_CONSTEXPR_11 200704L
#define RANGES_CXX_CONSTEXPR_14 201304L
#define RANGES_CXX_CONSTEXPR_17 201603L
#define RANGES_CXX_RANGE_BASED_FOR_11 200907L
#define RANGES_CXX_RANGE_BASED_FOR_14 RANGES_CXX_RANGE_BASED_FOR_11
#define RANGES_CXX_RANGE_BASED_FOR_17 201603L
#define RANGES_CXX_LIB_IS_FINAL_11 0L
#define RANGES_CXX_LIB_IS_FINAL_14 201402L
#define RANGES_CXX_LIB_IS_FINAL_17 RANGES_CXX_LIB_IS_FINAL_14
#define RANGES_CXX_RETURN_TYPE_DEDUCTION_11 0L
#define RANGES_CXX_RETURN_TYPE_DEDUCTION_14 201304L
#define RANGES_CXX_RETURN_TYPE_DEDUCTION_17 RANGES_CXX_RETURN_TYPE_DEDUCTION_14
#define RANGES_CXX_GENERIC_LAMBDAS_11 0L
#define RANGES_CXX_GENERIC_LAMBDAS_14 201304L
#define RANGES_CXX_GENERIC_LAMBDAS_17 RANGES_CXX_GENERIC_LAMBDAS_14
#define RANGES_CXX_STD_11 201103L
#define RANGES_CXX_STD_14 201402L
#define RANGES_CXX_STD_17 201703L
#define RANGES_CXX_THREAD_LOCAL_PRE_STANDARD 200000L // Arbitrary number between 0 and C++11
#define RANGES_CXX_THREAD_LOCAL_11 RANGES_CXX_STD_11
#define RANGES_CXX_THREAD_LOCAL_14 RANGES_CXX_THREAD_LOCAL_11
#define RANGES_CXX_THREAD_LOCAL_17 RANGES_CXX_THREAD_LOCAL_14
#define RANGES_CXX_INLINE_VARIABLES_11 0L
#define RANGES_CXX_INLINE_VARIABLES_14 0L
#define RANGES_CXX_INLINE_VARIABLES_17 201606L
#define RANGES_CXX_COROUTINES_11 0L
#define RANGES_CXX_COROUTINES_14 0L
#define RANGES_CXX_COROUTINES_17 0L
#define RANGES_CXX_COROUTINES_TS1 201703L
#define RANGES_CXX_DEDUCTION_GUIDES_11 0L
#define RANGES_CXX_DEDUCTION_GUIDES_14 0L
#define RANGES_CXX_DEDUCTION_GUIDES_17 201606L
#define RANGES_CXX_IF_CONSTEXPR_11 0L
#define RANGES_CXX_IF_CONSTEXPR_14 0L
#define RANGES_CXX_IF_CONSTEXPR_17 201606L
#define RANGES_CXX_ALIGNED_NEW_11 0L
#define RANGES_CXX_ALIGNED_NEW_14 0L
#define RANGES_CXX_ALIGNED_NEW_17 201606L

// Implementation-specific diagnostic control
#if defined(_MSC_VER) && !defined(__clang__)
#define RANGES_DIAGNOSTIC_PUSH __pragma(warning(push))
#define RANGES_DIAGNOSTIC_POP __pragma(warning(pop))
#define RANGES_DIAGNOSTIC_IGNORE_PRAGMAS __pragma(warning(disable:4068))
#define RANGES_DIAGNOSTIC_IGNORE(X) RANGES_DIAGNOSTIC_IGNORE_PRAGMAS __pragma(warning(disable:X))
#define RANGES_DIAGNOSTIC_IGNORE_SHADOWING RANGES_DIAGNOSTIC_IGNORE(4456)
#define RANGES_DIAGNOSTIC_IGNORE_INDENTATION
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
#define RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS RANGES_DIAGNOSTIC_IGNORE(4099)
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER
#define RANGES_DIAGNOSTIC_IGNORE_ZERO_LENGTH_ARRAY
#define RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
#define RANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define RANGES_DIAGNOSTIC_IGNORE_MISSING_BRACES
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE
#define RANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE
#define RANGES_DIAGNOSTIC_IGNORE_RANGE_LOOP_ANALYSIS
#define RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS RANGES_DIAGNOSTIC_IGNORE(4996)
#define RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE
// Ignores both "divide by zero" and "mod by zero":
#define RANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO \
    RANGES_DIAGNOSTIC_IGNORE(4723) RANGES_DIAGNOSTIC_IGNORE(4724)

#define RANGES_CXX_VER _MSVC_LANG

#if _MSC_VER < 1920
#define RANGES_WORKAROUND_MSVC_DC338193 // https://developercommunity.visualstudio.com/content/problem/338193/sfinae-disabled-ref-qualified-function-collides-wi.html
#define RANGES_WORKAROUND_MSVC_401490 // conversion of constant expressions with representable values is NOT narrowing
#define RANGES_WORKAROUND_MSVC_589046 // hidden friends should not be visible to qualified name lookup
#define RANGES_WORKAROUND_MSVC_701425 // Failure to deduce decltype(pointer-to-member) (gcc_bugs_bugs_bugs for MSVC)
#endif

#define RANGES_WORKAROUND_MSVC_249830 // constexpr and arguments that aren't subject to lvalue-to-rvalue conversion
#define RANGES_WORKAROUND_MSVC_620035 // Error when definition-context name binding finds only deleted function
#define RANGES_WORKAROUND_MSVC_677925 // Bogus C2676 "binary '++': '_Ty' does not define this operator"
#define RANGES_WORKAROUND_MSVC_683388 // decltype(*i) is incorrectly an rvalue reference for pointer-to-array i
#define RANGES_WORKAROUND_MSVC_688606 // SFINAE failing to account for access control during specialization matching
#define RANGES_WORKAROUND_MSVC_699982 // Nasty context-sensitive alias expansion / SFINAE error
#define RANGES_WORKAROUND_MSVC_701385 // Yet another alias expansion error

// Relocate the following into the <1920 section after VS2019 Preview 2 release:
#define RANGES_WORKAROUND_MSVC_711347
// MSVC doesn't define __cpp_coroutines even with /await
#if !defined(RANGES_CXX_COROUTINES) && defined(_RESUMABLE_FUNCTIONS_SUPPORTED)
#define RANGES_CXX_COROUTINES RANGES_CXX_COROUTINES_TS1
#endif

#elif defined(__GNUC__) || defined(__clang__)
#define RANGES_PRAGMA(X) _Pragma(#X)
#define RANGES_DIAGNOSTIC_PUSH RANGES_PRAGMA(GCC diagnostic push)
#define RANGES_DIAGNOSTIC_POP RANGES_PRAGMA(GCC diagnostic pop)
#define RANGES_DIAGNOSTIC_IGNORE_PRAGMAS RANGES_PRAGMA(GCC diagnostic ignored "-Wpragmas")
#define RANGES_DIAGNOSTIC_IGNORE(X) \
    RANGES_DIAGNOSTIC_IGNORE_PRAGMAS \
    RANGES_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas") \
    RANGES_PRAGMA(GCC diagnostic ignored "-Wunknown-warning-option") \
    RANGES_PRAGMA(GCC diagnostic ignored X)
#define RANGES_DIAGNOSTIC_IGNORE_SHADOWING RANGES_DIAGNOSTIC_IGNORE("-Wshadow")
#define RANGES_DIAGNOSTIC_IGNORE_INDENTATION RANGES_DIAGNOSTIC_IGNORE("-Wmisleading-indentation")
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL RANGES_DIAGNOSTIC_IGNORE("-Wundefined-internal")
#define RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS RANGES_DIAGNOSTIC_IGNORE("-Wmismatched-tags")
#define RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION RANGES_DIAGNOSTIC_IGNORE("-Wsign-conversion")
#define RANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL RANGES_DIAGNOSTIC_IGNORE("-Wfloat-equal")
#define RANGES_DIAGNOSTIC_IGNORE_MISSING_BRACES RANGES_DIAGNOSTIC_IGNORE("-Wmissing-braces")
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS RANGES_DIAGNOSTIC_IGNORE("-Wglobal-constructors")
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL RANGES_DIAGNOSTIC_IGNORE("-Wunneeded-internal-declaration")
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER RANGES_DIAGNOSTIC_IGNORE("-Wunneeded-member-function")
#define RANGES_DIAGNOSTIC_IGNORE_ZERO_LENGTH_ARRAY RANGES_DIAGNOSTIC_IGNORE("-Wzero-length-array")
#define RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT RANGES_DIAGNOSTIC_IGNORE("-Wc++1z-compat")
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE RANGES_DIAGNOSTIC_IGNORE("-Wundefined-func-template")
#define RANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE RANGES_DIAGNOSTIC_IGNORE("-Winconsistent-missing-override")
#define RANGES_DIAGNOSTIC_IGNORE_RANGE_LOOP_ANALYSIS RANGES_DIAGNOSTIC_IGNORE("-Wrange-loop-analysis")
#define RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS RANGES_DIAGNOSTIC_IGNORE("-Wdeprecated-declarations")
#define RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE RANGES_DIAGNOSTIC_IGNORE("-Wdeprecated-this-capture")
#define RANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO

#define RANGES_WORKAROUND_CWG_1554
#ifdef __clang__
#define RANGES_WORKAROUND_CLANG_37556
#else // __GNUC__
#if __GNUC__ < 6
#define RANGES_WORKAROUND_GCC_UNFILED0 /* Workaround old GCC name lookup bug */
#endif
#endif

#else
#define RANGES_DIAGNOSTIC_PUSH
#define RANGES_DIAGNOSTIC_POP
#define RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
#define RANGES_DIAGNOSTIC_IGNORE_SHADOWING
#define RANGES_DIAGNOSTIC_IGNORE_INDENTATION
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
#define RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
#define RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
#define RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_INTERNAL
#define RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER
#define RANGES_DIAGNOSTIC_IGNORE_ZERO_LENGTH_ARRAY
#define RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
#define RANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define RANGES_DIAGNOSTIC_IGNORE_MISSING_BRACES
#define RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE
#define RANGES_DIAGNOSTIC_IGNORE_INCONSISTENT_OVERRIDE
#define RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
#define RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE
#define RANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO
#endif

// Configuration via feature-test macros, with fallback to __cplusplus
#ifndef RANGES_CXX_VER
#define RANGES_CXX_VER __cplusplus
#endif

#define RANGES_CXX_FEATURE_CONCAT2(y, z) RANGES_CXX_ ## y ## _ ## z
#define RANGES_CXX_FEATURE_CONCAT(y, z) RANGES_CXX_FEATURE_CONCAT2(y, z)

#if RANGES_CXX_VER >= RANGES_CXX_STD_17
#define RANGES_CXX_STD RANGES_CXX_STD_17
#define RANGES_CXX_FEATURE(x) RANGES_CXX_FEATURE_CONCAT(x, 17)
#elif RANGES_CXX_VER >= RANGES_CXX_STD_14
#define RANGES_CXX_STD RANGES_CXX_STD_14
#define RANGES_CXX_FEATURE(x) RANGES_CXX_FEATURE_CONCAT(x, 14)
#else
#define RANGES_CXX_STD RANGES_CXX_STD_11
#define RANGES_CXX_FEATURE(x) RANGES_CXX_FEATURE_CONCAT(x, 11)
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

#if !defined(RANGES_DEPRECATED) && !defined(RANGES_DISABLE_DEPRECATED_WARNINGS)
#if RANGES_CXX_ATTRIBUTE_DEPRECATED &&            \
   !((defined(__clang__) || defined(__GNUC__)) && \
     RANGES_CXX_STD < RANGES_CXX_STD_14)
#define RANGES_DEPRECATED(MSG) [[deprecated(MSG)]]
#elif defined(__clang__) || defined(__GNUC__)
#define RANGES_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#endif
#endif
#ifndef RANGES_DEPRECATED
#define RANGES_DEPRECATED(MSG)
#endif

#ifndef RANGES_CXX_COROUTINES
#ifdef __cpp_coroutines
#define RANGES_CXX_COROUTINES __cpp_coroutines
#else
#define RANGES_CXX_COROUTINES RANGES_CXX_FEATURE(COROUTINES)
#endif
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

#ifndef RANGES_CXX_INLINE_VARIABLES
#ifdef __cpp_inline_variables
#define RANGES_CXX_INLINE_VARIABLES __cpp_inline_variables
#elif defined(__clang__) && (__clang_major__ == 3 && __clang_minor__ == 9) && \
    RANGES_CXX_VER > RANGES_CXX_STD_14
// Clang 3.9 supports inline variables in C++17 mode, but doesn't define __cpp_inline_variables
#define RANGES_CXX_INLINE_VARIABLES RANGES_CXX_INLINE_VARIABLES_17
#else
#define RANGES_CXX_INLINE_VARIABLES RANGES_CXX_FEATURE(INLINE_VARIABLES)
#endif  // __cpp_inline_variables
#endif  // RANGES_CXX_INLINE_VARIABLES

#if RANGES_CXX_INLINE_VARIABLES < RANGES_CXX_INLINE_VARIABLES_17
#define RANGES_INLINE_VARIABLE(type, name)                          \
    inline namespace                                                \
    {                                                               \
        constexpr auto &name = ::ranges::static_const<type>::value; \
    }
#else  // RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
#define RANGES_INLINE_VARIABLE(type, name) \
    inline constexpr type name{};
#endif // RANGES_CXX_INLINE_VARIABLES

#ifndef RANGES_CXX_DEDUCTION_GUIDES
#if defined(__clang__) && defined(__apple_build_version__)
// Apple's clang version doesn't do deduction guides very well.
#define RANGES_CXX_DEDUCTION_GUIDES 0
#elif defined(__cpp_deduction_guides)
#define RANGES_CXX_DEDUCTION_GUIDES __cpp_deduction_guides
#else
#define RANGES_CXX_DEDUCTION_GUIDES RANGES_CXX_FEATURE(DEDUCTION_GUIDES)
#endif // __cpp_deduction_guides
#endif // RANGES_CXX_DEDUCTION_GUIDES

#ifndef RANGES_CXX_IF_CONSTEXPR
#ifdef __cpp_if_constexpr
#define RANGES_CXX_IF_CONSTEXPR __cpp_if_constexpr
#else
#define RANGES_CXX_IF_CONSTEXPR RANGES_CXX_FEATURE(IF_CONSTEXPR)
#endif
#endif // RANGES_CXX_IF_CONSTEXPR

// Its not enough for the compiler to support this; the stdlib must support it too.
#ifndef RANGES_CXX_ALIGNED_NEW
#if (!defined(_LIBCPP_VERSION) || \
		(_LIBCPP_VERSION >= 4000 && !defined(_LIBCPP_HAS_NO_ALIGNED_ALLOCATION))) && \
    (!defined(__GLIBCXX__) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 7))
#if defined(__cpp_aligned_new)
#define RANGES_CXX_ALIGNED_NEW __cpp_aligned_new
#else
#define RANGES_CXX_ALIGNED_NEW RANGES_CXX_FEATURE(ALIGNED_NEW)
#endif
#else // _LIBCPP_VERSION < 4000 || __GLIBCXX__ < 20170502
#define RANGES_CXX_ALIGNED_NEW 0L
#endif
#endif // RANGES_CXX_ALIGNED_NEW

#ifdef RANGES_FEWER_WARNINGS
#define RANGES_DISABLE_WARNINGS                 \
    RANGES_DIAGNOSTIC_PUSH                      \
    RANGES_DIAGNOSTIC_IGNORE_SHADOWING          \
    RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL \
    RANGES_DIAGNOSTIC_IGNORE_INDENTATION        \
    RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT

#define RANGES_RE_ENABLE_WARNINGS RANGES_DIAGNOSTIC_POP
#else
#define RANGES_DISABLE_WARNINGS
#define RANGES_RE_ENABLE_WARNINGS
#endif

#if defined(__clang__)
#if __has_attribute(no_sanitize)
#define RANGES_INTENDED_MODULAR_ARITHMETIC \
  __attribute__((__no_sanitize__("unsigned-integer-overflow")))
#else
#define RANGES_INTENDED_MODULAR_ARITHMETIC
#endif
#else
#define RANGES_INTENDED_MODULAR_ARITHMETIC
#endif

#ifndef RANGES_CONSTEXPR_IF
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
#define RANGES_CONSTEXPR_IF constexpr
#else
#define RANGES_CONSTEXPR_IF
#endif
#endif // RANGES_CONSTEXPR_IF

#if !defined(RANGES_BROKEN_CPO_LOOKUP) && !defined(RANGES_DOXYGEN_INVOKED) && \
    (defined(RANGES_WORKAROUND_CLANG_37556) || \
     defined(RANGES_WORKAROUND_GCC_UNFILED0) || \
     defined(RANGES_WORKAROUND_MSVC_589046) || defined(RANGES_WORKAROUND_MSVC_620035))
#define RANGES_BROKEN_CPO_LOOKUP 1
#endif
#ifndef RANGES_BROKEN_CPO_LOOKUP
#define RANGES_BROKEN_CPO_LOOKUP 0
#endif

#ifndef RANGES_EMPTY_BASES
#ifdef _MSC_VER
#define RANGES_EMPTY_BASES __declspec(empty_bases)
#else
#define RANGES_EMPTY_BASES
#endif
#endif

#endif
