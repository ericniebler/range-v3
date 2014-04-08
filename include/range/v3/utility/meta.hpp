// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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

        template<template<typename...> class C>
        struct meta_quote
        {
            template<typename...Ts>
            using apply = typename C<Ts...>::type;
        };

        template<template<typename...> class C0, template<typename...> class C1>
        struct meta_compose
        {
            template<typename...Ts>
            using apply = C0<C1<Ts...>>;
        };

        template<template<typename...> class C, typename...As>
        using meta_apply = typename C<As...>::type;
    }
}

#endif
