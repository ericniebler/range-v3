/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_VIEW_COMPOSE_HPP
#define RANGE_V3_VIEW_COMPOSE_HPP

#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<template<class...> class View, class ...Args>
        struct compose_bind{
            template<class Rng>
            using view_t = View<Rng, Args...>;
        };

        template<class Src, template<class...> class Transformation, template<class...> class ...Transformations>
        struct compose_view{
            using type = Transformation< typename compose_view<Src, Transformations...>::type >;
        };

        template<class Src, template<class...> class Transformation>
        struct compose_view<Src, Transformation>{
            using type = Transformation< view::all_t<Src> >;
        };

    }
}

#endif //RANGE_V3_VIEW_COMPOSE_HPP
