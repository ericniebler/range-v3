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

#include <iostream>

namespace ranges
{
    inline namespace v3
    {
        namespace details{ namespace compose_view{



            namespace detail
            {
                template <std::size_t Ofst, class Tuple, std::size_t... I>
                constexpr auto slice_impl(Tuple&& t, std::index_sequence<I...>)
                {
                    return std::forward_as_tuple(
                        std::get<I + Ofst>(std::forward<Tuple>(t))...);
                }
            }

            template <std::size_t I1, std::size_t I2, class Cont>
            constexpr auto tuple_slice(Cont&& t)
            {
                static_assert(I2 >= I1, "invalid slice");
                static_assert(std::tuple_size<std::decay_t<Cont>>::value >= I2,
                    "slice index out of bounds");

                return detail::slice_impl<I1>(std::forward<Cont>(t),
                    std::make_index_sequence<I2 - I1>{});
            }




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

            struct compose_bind_base{};

            template<int n /* last index */, class Src, auto&...Transformations>
            struct compose_view_
            {
                constexpr static auto &last = get_n<n, Transformations...>::functor;

                using prev_compose_view = compose_view_<n-1, Src, Transformations...>;

                using type_ = std::invoke_result_t<decltype(last), typename prev_compose_view::type>;
                using type  = typename unwrap<type_>::type;

                CONCEPT_ASSERT(View<type>());

                template<class Rng, class Arg, class ...Args>
                static type build(Rng &&rng, Arg&& arg, Args&&...args)
                {
                    using last_t = std::decay_t<decltype(last)>;
                    constexpr const bool is_binding = std::is_base_of_v<compose_bind_base, last_t>;

                    if constexpr (is_binding){
                        if constexpr(!is_pipeable<std::decay_t<Arg>>::value){
                            // make binding

                            auto args_tuple = std::forward_as_tuple(std::forward<Arg>(arg), std::forward<Args>(args)...);
                            constexpr const std::size_t tuple_size = sizeof...(Args)+1;

                            auto binded_view = std::apply(last_t::view, tuple_slice<0, last_t::args_size>(args_tuple));

                            auto prev_build = [](auto&&...args) -> decltype(auto) {
                                return prev_compose_view::build( std::forward<decltype(args)>(args)... );
                            };

                            return std::invoke(std::move(binded_view),
                               //prev_compose_view::build( std::forward<Rng>(rng), std::forward<Args>(args)... )
                                std::apply(prev_build,
                                    std::tuple_cat(
                                        std::forward_as_tuple(std::forward<Rng>(rng)),
                                        tuple_slice<last_t::args_size, tuple_size>(args_tuple)
                                    )
                                )
                            );
                        } else {
                            return std::invoke(std::forward<Arg>(arg),
                               prev_compose_view::build( std::forward<Rng>(rng), std::forward<Args>(args)... )
                            );
                        }
                    } else {
                        return std::invoke(last,
                           prev_compose_view::build( std::forward<Rng>(rng), std::forward<Arg>(arg), std::forward<Args>(args)... )
                        );
                    }
                }

                template<class Rng>
                static type build(Rng &&rng)
                {
                    return std::invoke(last,
                        prev_compose_view::build( std::forward<Rng>(rng) )
                    );
                }
            };

            template<class Src, auto&...Transformations>
            struct compose_view_<0, Src, Transformations...>
            {
                CONCEPT_ASSERT(Range<Src>());

                constexpr static auto &last = get_n<0, Transformations...>::functor;

                using type_ = std::invoke_result_t<decltype(last), Src&>;
                using type  = typename unwrap<type_>::type;

                CONCEPT_ASSERT(View<type>());

                template<class Rng, class Arg>
                static type build(Rng &&rng, Arg&& arg)
                {
                    using last_t = std::decay_t<decltype(last)>;
                    constexpr const bool is_binding = std::is_base_of_v<compose_bind_base, last_t>;
                    static_assert(is_binding);
                    if constexpr(!is_pipeable<std::decay_t<Arg>>::value){
                        static_assert(last_t::args_size == 1);
                        auto binded_view = last_t::view(std::forward<Arg>(arg));
                        return std::invoke(std::move(binded_view), std::forward<Rng>(rng));
                    } else {
                        return std::invoke(std::forward<Arg>(arg), std::forward<Rng>(rng));
                    }
                }
                template<class Rng>
                static type build(Rng &&rng)
                {
                    return std::invoke(last, std::forward<Rng>(rng));
                }
            };

            template<class Src, auto&...Transformations>
            using compose_view = compose_view_<sizeof...(Transformations) - 1, Src, Transformations...>;

            template<auto& View, class ...Args>
            struct compose_bind_fn : compose_bind_base {
                static constexpr const auto args_size = sizeof...(Args);
                static constexpr auto& view = View;

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
            template<class...Bindings>
            explicit compose_view(Src& src, Bindings&& ...bindings)
                : Base( composed_view::build(src, std::forward<Bindings>(bindings)...) )
            {}
        };

        template<auto& View, class ...Args>
        constexpr details::compose_view::compose_bind_fn<View, Args...> compose_bind{};
    }
}
#endif //RANGES_CXX_VER >= RANGES_CXX_STD_17

#endif //RANGE_V3_VIEW_COMPOSE_HPP
