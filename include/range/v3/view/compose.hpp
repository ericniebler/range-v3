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
        namespace details{ namespace compose_view{
            template<int n, template<class> class Transformation, template<class> class ...Transformations>
            struct get_n
            {
                template<class Arg>
                using type = typename get_n<n-1, Transformations...>::template type<Arg>;
            };

            template<template<class> class Transformation, template<class> class ...Transformations>
            struct get_n<0, Transformation, Transformations...>
            {
                template<class Arg>
                using type = Transformation<Arg>;
            };

            template<int n /* last index */, class Src, template<class> class ...Transformations>
            struct compose_view_
            {
                template<class T>
                using last = typename get_n<n, Transformations...>::template type<T>;

                using prev_compose_view = compose_view_<n-1, Src, Transformations...>;

                using type = last< typename prev_compose_view::type >;

                CONCEPT_ASSERT(View<type>());

                template<class Rng>
                static type build(Rng&& rng)
                {
                    return type(prev_compose_view::build( std::forward<Rng>(rng) ));
                }
            };

            template<class Src, template<class> class ...Transformations>
            struct compose_view_<0, Src, Transformations...>
            {
                CONCEPT_ASSERT(Range<Src>());

                template<class T>
                using last = typename get_n<0, Transformations...>::template type<T>;

                using type = last<view::all_t<Src>>;

                CONCEPT_ASSERT(View<type>());

                template<class Rng>
                static type build(Rng&& rng)
                {
                    return type(view::all(std::forward<Rng>(rng)));
                }
            };

            template<class Src, template<class> class ...Transformations>
            using compose_view = compose_view_<sizeof...(Transformations)-1, Src, Transformations...>;
        }}

        // hide this?
        template<class Src, template<class> class ...Transformations>
        using compose_view_t = typename details::compose_view::compose_view<Src, Transformations...>::type;

        template<class Src, template<class> class ...Transformations>
        struct compose_view : compose_view_t<Src, Transformations...>
        {
        private:
            using Base          = compose_view_t<Src, Transformations...>;
            using composed_view = details::compose_view::compose_view<Src, Transformations...>;
        public:
            using type = Base;

            compose_view() = default;

            // implicit conversion constructor
            compose_view(Base other)
                : Base(std::move(other))
            {}

            explicit compose_view(Src&& src)
                : Base( composed_view::build(std::move(src)) )
            {}
            explicit compose_view(const Src& src)
                : Base( composed_view::build(src) )
            {}
            explicit compose_view(Src& src)
                : Base( composed_view::build(src) )
            {}
        };

        template<template<class...> class View, class Arg>
        struct compose_bind
        {
            template<class Rng>
            using type = View<Rng, Arg>;
        };
    }
}

#endif //RANGE_V3_VIEW_COMPOSE_HPP
