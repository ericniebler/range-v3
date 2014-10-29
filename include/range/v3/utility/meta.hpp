// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_META_HPP
#define RANGES_V3_UTILITY_META_HPP

namespace ranges
{
    inline namespace v3
    {
        ////////////////////////////////////////////////////////////////////////////////////
        // General meta-programming utilities
        template<template<typename...> class C, typename...Ts>
        struct meta_bind_front
        {
            template<typename...Us>
            using apply = C<Ts..., Us...>;
        };

        template<template<typename...> class C, typename...Us>
        struct meta_bind_back
        {
            template<typename...Ts>
            using apply = C<Ts..., Us...>;
        };

        template<typename T>
        using meta_eval = typename T::type;

#if __GNUC__ == 4 && __GNUC_MINOR__ <= 9
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61738
        template<template<typename...> class C, typename...As>
        using meta_apply = typename C<As...>::type;
#else
        template<template<typename...> class C, typename...As>
        using meta_apply = meta_eval<C<As...>>;
#endif

        template<template<typename...> class C>
        struct meta_quote
        {
            template<typename...Ts>
            using apply = meta_apply<C, Ts...>;
        };

        template<template<typename...> class C0, template<typename...> class C1>
        struct meta_compose
        {
            template<typename...Ts>
            using apply = C0<C1<Ts...>>;
        };

        template<template<typename...> class C, typename...As>
        struct meta_defer
          : C<As...>
        {};
    }
}

#endif
