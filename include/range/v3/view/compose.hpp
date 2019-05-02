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

#include <type_traits>
#include <utility>
#include <tuple>
#include <range/v3/range_concepts.hpp>

#if RANGES_CXX_VER >= RANGES_CXX_STD_17

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            namespace compose_view
            {

                namespace detail
                {
                    template <std::size_t Offset, class Tuple, std::size_t... Is>
                    constexpr auto tuple_slice_(Tuple&& tuple, std::index_sequence<Is...>)
                    {
                        return std::forward_as_tuple(
                            std::get<Is + Offset>(std::forward<Tuple>(tuple))...);
                    }
                }
                // [From;To)
                template <std::size_t From, std::size_t To, class Tuple>
                constexpr auto tuple_slice(Tuple&& tuple)
                {
                    static_assert(From <= To);
                    static_assert(To   <= std::tuple_size_v<std::decay_t<Tuple>>);

                    return detail::tuple_slice_<From>(std::forward<Tuple>(tuple),
                        std::make_index_sequence<To - From>{});
                }

                template<int n, auto &Transformation, auto&...Transformations>
                constexpr auto& get_n()
                {
                    if constexpr(n==0)
                    {
                        return Transformation;
                    }
                    else
                    {
                        return get_n<n-1,Transformations...>();
                    }
                }

                struct type_wrapper_base {};
                template<class T>
                struct type_wrapper : type_wrapper_base { using type = T; };

                template<class T, bool is_wrapped = std::is_base_of_v<type_wrapper_base, T> >
                struct unwrap;
                template<class T>
                struct unwrap<T, true>  { using type = typename T::type; };
                template<class T>
                struct unwrap<T, false> { using type = T; };

                struct compose_bind_base{};

                template<auto i>
                static const constexpr std::integral_constant<decltype(i), i> integral_constant{};

                // compose in reverse order
                template<int n /* last index */, class Src, auto&...Transformations>
                struct compose_view_
                {
                    CONCEPT_ASSERT(Range<Src>());

                    constexpr static auto &last = get_n<n, Transformations...>();

                    using prev_compose_view = compose_view_<n-1, Src, Transformations...>;

                    template<int j, class dummy = void>
                    struct get_type
                    {
                        using type = invoke_result_t<decltype(last), typename prev_compose_view::type>;
                    };
                    template<class dummy>
                    struct get_type<0, dummy>
                    {
                        using type = invoke_result_t<decltype(last), Src&>;
                    };

                    using type  = typename unwrap<typename get_type<n>::type>::type;

                    CONCEPT_ASSERT(View<type>());

                    template<int arg_index /*= std::tuple_size_v<ArgsTuple>-1*/, class Rng, class ArgsTuple >
                    static type build(Rng &&rng, ArgsTuple&& args_tuple)
                    {
                        using last_t = std::decay_t<decltype(last)>;
                        constexpr const bool is_binding = std::is_base_of_v<compose_bind_base, last_t>;

                        auto prev_compose_view_build = [&](auto index_t) -> decltype(auto)
                        {
                            if constexpr (n<=0)
                            {
                                return std::forward<Rng>(rng);
                            }
                            else
                            {
                                constexpr const auto index = decltype(index_t)::value;
                                return prev_compose_view::template build<index>(std::forward<Rng>(rng),std::forward<ArgsTuple>(args_tuple));
                            }
                        };

                        if constexpr (is_binding && std::tuple_size_v<ArgsTuple> > 0)
                        {
                            static_assert(arg_index >= 0);
                            auto&& first_arg = std::get<arg_index>(args_tuple);
                            using  FirstArg  = decltype(first_arg);

                            if constexpr(!is_pipeable<std::decay_t<FirstArg>>::value)
                            {
                                // make binding from arguments
                                constexpr const int from = arg_index+1-last_t::args_count;
                                constexpr const int to   = arg_index+1;
                                auto binded_view = std::apply(last_t::view, tuple_slice<from, to>(args_tuple));

                                return std::invoke(std::move(binded_view),
                                   prev_compose_view_build(integral_constant<arg_index - last_t::args_count>)
                                );
                            }
                            else
                            {
                                // argument is a bind
                                return std::invoke(std::forward<FirstArg>(first_arg),
                                   prev_compose_view_build(integral_constant<arg_index-1>)
                                );
                            }
                        }
                        else
                        {
                            // just compose
                            return std::invoke(last,
                               prev_compose_view_build(integral_constant<arg_index>)
                            );
                        }
                    }

                    template<class Rng, class... Bindings>
                    static type build(Rng &&rng, Bindings&& ...bindings)
                    {
                        return build<int(sizeof...(Bindings))-1>(
                            std::forward<Rng>(rng),
                            std::forward_as_tuple(std::forward<Bindings>(bindings)...)
                        );
                    }
                };

                template<class Src, auto&...Transformations>
                using compose_view = compose_view_<sizeof...(Transformations) - 1, Src, Transformations...>;

                template<auto& View, class ...Args>
                struct compose_bind_fn : compose_bind_base
                {
                    static constexpr const int args_count = sizeof...(Args);
                    static constexpr auto& view = View;

                    template<class Rng>
                    using result_t = std::invoke_result_t<decltype(View), Rng, Args...>;

                    template<class Rng>
                    type_wrapper< result_t<Rng> >
                    operator()(Rng&&) const { return {}; }
                };
            } // namespace compose_view
        } // namespace detail
        /// \endcond

        template<class Src, auto& ...Transformations>
        using compose_view_t = typename detail::compose_view::compose_view<Src, Transformations...>::type;

        template<class Src, auto& ...Transformations>
        struct compose_view : compose_view_t<Src, Transformations...>
        {
        private:
            using Base          = compose_view_t<Src, Transformations...>;
            using composed_view = detail::compose_view::compose_view<Src, Transformations...>;
        public:
            using type = Base;

            compose_view() = default;

            // implicit conversion constructor
            compose_view(Base other)
                : Base(std::move(other))
            {}

            template<class...Bindings>
            explicit compose_view(Src&& src, Bindings&& ...bindings)
                : Base(composed_view::build(std::move(src), std::forward<Bindings>(bindings)...))
            {}
            template<class...Bindings>
            explicit compose_view(const Src& src, Bindings&& ...bindings)
                : Base(composed_view::build(src, std::forward<Bindings>(bindings)...))
            {}
            template<class...Bindings>
            explicit compose_view(Src& src, Bindings&& ...bindings)
                : Base(composed_view::build(src, std::forward<Bindings>(bindings)...))
            {}
        };

        template<auto& View, class ...Args>
        constexpr detail::compose_view::compose_bind_fn<View, Args...> compose_bind{};

        RANGES_DIAGNOSTIC_PUSH
        RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS

        template<auto& View, auto...Args>
        auto compose_bind_static = View(Args...);

        RANGES_DIAGNOSTIC_POP
    }
}
#endif //RANGES_CXX_VER >= RANGES_CXX_STD_17

#endif //RANGE_V3_VIEW_COMPOSE_HPP
