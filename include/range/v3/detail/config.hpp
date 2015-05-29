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

#ifndef RANGES_V3_DETAIL_CONFIG_HPP
#define RANGES_V3_DETAIL_CONFIG_HPP

#include <iosfwd>

#ifndef RANGES_ASSERT
# include <cassert>
# define RANGES_ASSERT assert
#endif

#ifndef RANGES_ENSURE_MSG
# include <exception>
# define RANGES_ENSURE_MSG(COND, MSG) \
    ((COND) ? void() : (RANGES_ASSERT(!(true && MSG)), std::terminate()))
#endif

#ifndef RANGES_ENSURE
# define RANGES_ENSURE(COND) \
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

// Non-portable forward declarations of standard containers
#ifdef _LIBCPP_VERSION
#define RANGES_BEGIN_NAMESPACE_STD _LIBCPP_BEGIN_NAMESPACE_STD
#define RANGES_END_NAMESPACE_STD _LIBCPP_END_NAMESPACE_STD
#else
#define RANGES_BEGIN_NAMESPACE_STD namespace std {
#define RANGES_END_NAMESPACE_STD }
#endif

#ifdef __clang__
#define RANGES_CXX_NO_VARIABLE_TEMPLATES !__has_feature(cxx_variable_templates)
#else
#define RANGES_CXX_NO_VARIABLE_TEMPLATES 1
#endif

#ifndef RANGES_THREAD_LOCAL
#if (defined(__clang__) && defined(__CYGWIN__)) || \
    (defined(__clang__) && defined(_LIBCPP_VERSION)) // BUGBUG avoid unresolved __cxa_thread_atexit
#define RANGES_STATIC_THREAD_LOCAL
#else
#define RANGES_STATIC_THREAD_LOCAL static thread_local
#endif
#endif

#if __cplusplus > 201103
#define RANGES_CXX_GREATER_THAN_11
#endif

#if __cplusplus > 201402
#define RANGES_CXX_GREATER_THAN_14
#endif

#ifndef RANGES_DISABLE_DEPRECATED_WARNINGS
#ifdef RANGES_CXX_GREATER_THAN_11
#define RANGES_DEPRECATED(MSG) [[deprecated(MSG)]]
#else
#if defined(__clang__) || defined(__GNUC__)
#define RANGES_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#elif defined(_MSC_VER)
#define RANGES_DEPRECATED(MSG) __declspec(deprecated(MSG))
#else
#define RANGES_DEPRECATED(MSG)
#endif
#endif
#else
#define RANGES_DEPRECATED(MSG)
#endif

// RANGES_CXX14_CONSTEXPR macro (see also BOOST_CXX14_CONSTEXPR)
// Note: constexpr implies inline, to retain the same visibility
// C++14 constexpr functions are inline in C++11
#ifdef RANGES_CXX_GREATER_THAN_11
#define RANGES_CXX14_CONSTEXPR constexpr
#else
#define RANGES_CXX14_CONSTEXPR inline
#endif

#endif
