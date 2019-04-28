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

#if RANGES_CXX_VER >= RANGES_CXX_STD_17

#include <type_traits>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace details{ namespace compose_view{

            template<int n, auto &Transformation, auto&...Transformations>
            struct get_n
            {
                constexpr static auto &functor = get_n<n-1, Transformations...>::functor;
            };

            template<auto &Transformation, auto&...Transformations>
            struct get_n<0, Transformation, Transformations...>
            {
                constexpr static auto &functor = Transformation;
            };

            struct type_wrapper_base {};
            template<class T>
            struct type_wrapper : type_wrapper_base
            {
                using type = T;
            };

            template<class T, bool is_wrapped = std::is_base_of_v<type_wrapper_base, T> >
            struct unwrap;

            template<class T>
            struct unwrap<T, true>
            {
                using type = typename T::type;
            };
            template<class T>
            struct unwrap<T, false>
            {
                using type = T;
            };


            template<int n /* last index */, class Src, auto&...Transformations>
            struct compose_view_
            {
                constexpr static auto &last = get_n<n, Transformations...>::functor;

                using prev_compose_view = compose_view_<n-1, Src, Transformations...>;

                using type_ = std::invoke_result_t<decltype(last), typename prev_compose_view::type>;
                using type  = typename unwrap<type_>::type;

                CONCEPT_ASSERT(View<type>());

                template<class Rng>
                static type build(Rng &&rng)
                {
                    return std::invoke(last, prev_compose_view::build(std::forward<Rng>(rng)));
                }
            };

            template<class Src, auto&...Transformations>
            struct compose_view_<0, Src, Transformations...>
            {
                CONCEPT_ASSERT(Range<Src>());

                constexpr static auto &last = get_n<0, Transformations...>::functor;

                using type_ = std::invoke_result_t<decltype(last), view::all_t<Src>>;
                using type  = typename unwrap<type_>::type;

                CONCEPT_ASSERT(View < type > ());

                template<class Rng>
                static type build(Rng &&rng)
                {
                    return std::invoke(last, view::all(std::forward<Rng>(rng)));
                }
            };

            template<class Src, auto&...Transformations>
            using compose_view = compose_view_<sizeof...(Transformations) - 1, Src, Transformations...>;

            template<auto& View, class ...Args>
            struct compose_bind_fn {
                template<class Rng>
                decltype(auto) operator()(Rng&& rng) const {
                    constexpr const bool all_default_constructible = (std::is_default_constructible_v<Args> && ...);
                    if constexpr (all_default_constructible){
                        return std::invoke(View, std::forward<Rng>(rng), Args()...);
                    } else {
                        return type_wrapper< std::invoke_result_t<decltype(View), Rng, Args...> >{};
                    }
                }
            };
        }}

        template<class Src, auto& ...Transformations>
        using compose_view_t = typename details::compose_view::compose_view<Src, Transformations...>::type;


        template<class Src, auto& ...Transformations>
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

        template<auto& View, class ...Args>
        constexpr details::compose_view::compose_bind_fn<View, Args...> compose_bind{};
    }
}
#endif //RANGES_CXX_VER >= RANGES_CXX_STD_17

#endif //RANGE_V3_VIEW_COMPOSE_HPP
