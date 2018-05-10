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

#include <utility>

#define META_CXX_STD_14 201402L

#if defined(_MSC_VER) && defined(_MSVC_LANG)
#define META_CXX_VER _MSVC_LANG
#define META_HAS_MAKE_INTEGER_SEQ 1
#else
#define META_CXX_VER __cplusplus
#endif

#ifndef META_CXX_VARIABLE_TEMPLATES
#ifdef __cpp_variable_templates
#define META_CXX_VARIABLE_TEMPLATES __cpp_variable_templates
#else
#define META_CXX_VARIABLE_TEMPLATES (META_CXX_VER >= META_CXX_STD_14)
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

namespace meta
{
    inline namespace v1
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

        template <typename... Fs>
        struct compose;

        template <template <typename...> class C, typename... Ts>
        struct defer;

        template <typename T, template <T...> class C, T... Is>
        struct defer_i;

        namespace extension
        {
            template <typename F, typename List>
            struct apply;
        }

    } // inline namespace v1
} // namespace meta

#endif
