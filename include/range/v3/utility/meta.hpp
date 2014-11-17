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

#include <range/v3/utility/logical_ops.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename...T>
        struct typelist;

        ////////////////////////////////////////////////////////////////////////////////////
        // General meta-programming utilities
        template<typename T>
        using meta_eval = typename T::type;

        template<typename F, typename...Args>
        using meta_apply = typename F::template apply<Args...>;

        template<typename F, typename...Args>
        struct meta_apply_lazy
        {
            using type = meta_apply<F, Args...>;
        };

        template<template<typename...> class C>
        struct meta_quote
        {
        private:
            // Indirection here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template<typename...Ts>
            struct impl
            {
                using type = C<Ts...>;
            };
        public:
            template<typename...Ts>
            using apply = meta_eval<impl<Ts...>>;
        };

        template<template<typename...> class C>
        struct meta_quote_trait
        {
            template<typename...Ts>
            using apply = meta_eval<meta_apply<meta_quote<C>, Ts...> >;
        };

        template<typename T, template<T...> class F>
        struct meta_quote_i
        {
        private:
            // Indirection here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template<typename ...Ts>
            struct impl
            {
                using type = F<Ts::value...>;
            };
        public:
            template<typename...Ts>
            using apply = meta_eval<impl<Ts...>>;
        };

        template<typename T, template<T...> class C>
        struct meta_quote_trait_i
        {
            template<typename...Ts>
            using apply = meta_eval<meta_apply<meta_quote_i<T, C>, Ts...> >;
        };

        template<typename...Fs>
        struct meta_compose
        {};

        template<typename F0>
        struct meta_compose<F0>
        {
            template<typename...Ts>
            using apply = meta_apply<F0, Ts...>;
        };

        template<typename F0, typename...Fs>
        struct meta_compose<F0, Fs...>
        {
            template<typename...Ts>
            using apply = meta_apply<F0, meta_apply<meta_compose<Fs...>, Ts...>>;
        };

        struct meta_id
        {
            template<typename T>
            using apply = T;
        };

        template<typename T>
        struct meta_always
        {
            template<typename...>
            using apply = T;
        };

        template<typename F, typename...Ts>
        struct meta_bind_front
        {
            template<typename...Us>
            using apply = meta_apply<F, Ts..., Us...>;
        };

        template<typename F, typename...Us>
        struct meta_bind_back
        {
            template<typename...Ts>
            using apply = meta_apply<F, Ts..., Us...>;
        };

        template<typename F, typename Q = meta_quote<typelist>>
        struct meta_curry
          : meta_compose<F, Q>
        {};

        template<typename F>
        struct meta_uncurry
        {
        private:
            template<typename T>
            struct impl
            {};
            template<template<typename...> class T, typename ...Ts>
            struct impl<T<Ts...>>
            {
                using type = meta_apply<F, Ts...>;
            };
        public:
            template<typename T>
            using apply = meta_eval<impl<T>>;
        };

        template<typename F>
        struct meta_flip
        {
        private:
            template<typename ...Ts>
            struct impl
            {};
            template<typename A, typename B, typename ...Ts>
            struct impl<A, B, Ts...>
            {
                using type = meta_apply<F, B, A, Ts...>;
            };
        public:
            template<typename ...Ts>
            using apply = meta_eval<impl<Ts...>>;
        };

        using meta_and = meta_quote<logical_and>;
        using meta_or  = meta_quote<logical_or>;
        using meta_not = meta_quote<logical_not>;
    }
}

#endif
