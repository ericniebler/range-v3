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

#ifndef RANGES_V3_CONTAINER_ACTION_HPP
#define RANGES_V3_CONTAINER_ACTION_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/pipeable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace action
        {
            struct action_access
            {
                template<typename Action>
                struct impl
                {
                    template<typename...Ts, typename A = Action>
                    static auto bind(Ts &&...ts)
                    RANGES_DECLTYPE_AUTO_RETURN
                    (
                        A::bind(std::forward<Ts>(ts)...)
                    )
                };
            };

            struct make_action_fn
            {
                template<typename Fun>
                action<Fun> operator()(Fun fun) const
                {
                    return {std::move(fun)};
                }
            };

            constexpr make_action_fn make_action{};

            template<typename Action>
            struct action : pipeable<action<Action>>
            {
            private:
                Action action_;
                friend pipeable_access;
                // Pipeing requires things are passed by value.
                template<typename Rng, typename Act,
                    CONCEPT_REQUIRES_(Function<Action, Rng>() && Iterable<Rng>() &&
                        !std::is_reference<Rng>())>
                static uncvref_t<Rng> pipe(Rng && rng, Act && act)
                {
                    act.action_(rng);
                    return std::forward<Rng>(rng);
                }
            public:
                action() = default;
                action(Action a)
                  : action_(std::move(a))
                {}
                // Calling directly requires things are passed by reference.
                template<typename Rng, typename...Rest,
                    CONCEPT_REQUIRES_(Iterable<Rng &>() && Function<Action, Rng &, Rest &&...>())>
                Rng & operator()(Rng & rng, Rest &&... rest) const
                {
                    action_(rng, std::forward<Rest>(rest)...);
                    return rng;
                }
                // Currying overload.
                template<typename T, typename...Rest, typename A = Action>
                auto operator()(T && t, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_action(action_access::impl<A>::bind(action_, std::forward<T>(t),
                        std::forward<Rest>(rest)...))
                )
            };

            template<typename Rng, typename Action,
                CONCEPT_REQUIRES_(Iterable<Rng &>() && Function<bitwise_or, ref_t<Rng &> &&, Action>() &&
                    is_pipeable<Action>())>
            Rng & operator|=(Rng & rng, Action && action)
            {
                ref(rng) | action;
                return rng;
            }
        }
    }
}

#endif
