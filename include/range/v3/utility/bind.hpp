// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_BIND_HPP
#define RANGES_V3_UTILITY_BIND_HPP

#include <tuple>
#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/integer_sequence.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            // is_bind_expression
            template<typename T>
            struct is_bind_expression_
              : std::false_type
            {};
        }

        template<typename T>
        struct is_bind_expression
          : detail::is_bind_expression_<detail::uncvref_t<T>>
        {};

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            // mu
            RANGES_CONSTEXPR struct mu_fn
            {
            private:
                template<typename T, typename Args, std::size_t...Is>
                static auto apply_(T && t, Args && args, integer_sequence<Is...>) ->
                    decltype(std::declval<T>()(std::get<Is>(std::declval<Args>())...))
                {
                    return std::forward<T>(t)(std::get<Is>(std::forward<Args>(args))...);
                }
            public:
                template<typename T, typename Args,
                    typename Tt = uncvref_t<T>,
                    typename std::enable_if<std::is_placeholder<Tt>::value == 0 &&
                                            !is_bind_expression<Tt>::value, int>::type = 0>
                T && operator()(T && t, Args &&) const
                {
                    return std::forward<T>(t);
                }

                template<typename T, typename Args,
                    typename Tt = uncvref_t<T>,
                    typename std::enable_if<std::is_placeholder<Tt>::value != 0, int>::type = 0>
                auto operator()(T && t, Args && args) const
                    -> decltype(std::get<std::is_placeholder<Tt>::value - 1>(std::declval<Args>()))
                {
                    return std::get<std::is_placeholder<Tt>::value - 1>(std::forward<Args>(args));
                }

                template<typename T, typename Args,
                    typename Tt = uncvref_t<T>,
                    typename std::enable_if<is_bind_expression<Tt>::value, int>::type = 0>
                auto operator()(T && t, Args && args) const
                    -> decltype(mu_fn::apply_(std::declval<T>(), std::declval<Args>(),
                            integer_sequence_t<std::tuple_size<uncvref_t<Args>>::value>{}))
                {
                    return mu_fn::apply_(std::forward<T>(t), std::forward<Args>(args),
                        integer_sequence_t<std::tuple_size<uncvref_t<Args>>::value>{});
                }
            } mu {};

            ////////////////////////////////////////////////////////////////////////////////////////
            // bind_apply
            RANGES_CONSTEXPR struct bind_apply_fn
            {
                template<typename Fn, typename BoundArgs, typename Args, std::size_t...Is>
                auto operator()(Fn && fn, BoundArgs && bound_args, Args && args,
                                integer_sequence<Is...>) const
                    -> decltype(std::declval<Fn>()(
                           detail::mu(std::get<Is>(std::declval<BoundArgs>()),
                                      std::declval<Args>())...))
                {
                    return std::forward<Fn>(fn)(
                        detail::mu(std::get<Is>(std::forward<BoundArgs>(bound_args)),
                                   std::forward<Args>(args))...);
                }
            } bind_apply {};

            ////////////////////////////////////////////////////////////////////////////////////////
            // binder
            template<typename Fn, typename ...BoundArgs>
            struct binder
            {
            private:
                invokable_t<Fn> fn_;
                std::tuple<BoundArgs...> bound_args_;
                using bound_arg_indices = integer_sequence_t<sizeof...(BoundArgs)>;
                friend struct ranges::bind_fn;

                explicit binder(Fn && fn, BoundArgs &&... args)
                  : fn_(invokable(std::forward<Fn>(fn)))
                  , bound_args_{std::forward<BoundArgs>(args)...}
                {}
            public:
                template<typename ...Args2>
                auto operator()(Args2 &&...args2) &
                    -> decltype(bind_apply(std::declval<binder &>().fn_,
                                           std::declval<binder &>().bound_args_,
                                           std::declval<std::tuple<Args2 &&...>>(),
                                           std::declval<bound_arg_indices>()))
                {
                    return bind_apply(fn_,
                                      bound_args_,
                                      std::forward_as_tuple(std::forward<Args2>(args2)...),
                                      bound_arg_indices{});
                }

                template<typename ...Args2>
                auto operator()(Args2 &&...args2) const &
                    -> decltype(bind_apply(std::declval<binder const &>().fn_,
                                           std::declval<binder const &>().bound_args_,
                                           std::declval<std::tuple<Args2 &&...>>(),
                                           std::declval<bound_arg_indices>()))
                {
                    return bind_apply(fn_,
                                      bound_args_,
                                      std::forward_as_tuple(std::forward<Args2>(args2)...),
                                      bound_arg_indices{});
                }

                template<typename ...Args2>
                auto operator()(Args2 &&...args2) &&
                    -> decltype(bind_apply(std::declval<binder>().fn_,
                                           std::declval<binder>().bound_args_,
                                           std::declval<std::tuple<Args2 &&...>>(),
                                           std::declval<bound_arg_indices>()))
                {
                    return bind_apply(std::move(*this).fn_,
                                      std::move(*this).bound_args_,
                                      std::forward_as_tuple(std::forward<Args2>(args2)...),
                                      bound_arg_indices{});
                }
            };

            template<typename Fn, typename...BoundArgs>
            struct is_bind_expression_<binder<Fn, BoundArgs...>>
              : std::true_type
            {};
        }

        struct bind_fn
        {
            template<typename Fn, typename... BoundArgs>
            detail::binder<Fn, BoundArgs...> operator()(Fn && fn, BoundArgs &&... args) const
            {
                return detail::binder<Fn, BoundArgs...>{std::forward<Fn>(fn),
                                                        std::forward<BoundArgs>(args)...};
            }
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // A version of std::bind that does perfect forwarding instead of relying on std::ref
        RANGES_CONSTEXPR bind_fn bind {};
    }
}

#endif
