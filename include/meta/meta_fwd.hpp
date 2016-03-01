/// \file meta_fwd.hpp Forward declarations
//
// Meta library
//
//  Copyright Eric Niebler 2014-2015
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

#ifndef META_DISABLE_DEPRECATED_WARNINGS
#ifdef __cpp_attribute_deprecated
#define META_DEPRECATED(MSG) [[deprecated(MSG)]]
#else
#if defined(__clang__) || defined(__GNUC__)
#define META_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#elif defined(_MSC_VER)
#define META_DEPRECATED(MSG) __declspec(deprecated(MSG))
#else
#define META_DEPRECATED(MSG)
#endif
#endif
#else
#define META_DEPRECATED(MSG)
#endif

namespace meta
{
    inline namespace v1
    {
#ifdef __cpp_lib_integer_sequence
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
