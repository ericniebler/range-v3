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
#include <range/v3/container/concepts.hpp>
#include <range/v3/utility/pipeable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace cont
        {
            template<typename Action>
            struct action : pipeable<action<Action>>
            {
            private:
                friend pipeable_access;
                // Pipeing requires things are passed by value.
                template<typename Rng, typename Act,
                    CONCEPT_REQUIRES_(Function<Action, Rng &>() && Iterable<Rng>() &&
                        !std::is_reference<Rng>())>
                static detail::uncvref_t<Rng> pipe(Rng && rng, Act && act)
                {
                    Action{}(rng);
                    return std::forward<Rng>(rng);
                }
            public:
                // Calling directly requires things are passed by reference.
                template<typename Rng, typename...Rest,
                    CONCEPT_REQUIRES_(Function<Action, Rng &, Rest...>() && Iterable<Rng>())>
                Rng & operator()(Rng & rng, Rest &&... rest) const
                {
                    Action{}(rng, std::forward<Rest>(rest)...);
                    return rng;
                }
            };

            template<typename Rng, typename Action,
                typename RngRef =
                    detail::conditional_t<(Range<Rng>()), Rng &, reference_wrapper<Rng>>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && Function<bitwise_or, RngRef, Action>() &&
                    is_pipeable<Action>())>
            Rng & operator|=(Rng & rng, Action && action)
            {
                RngRef{rng} | action;
                return rng;
            }
        }
    }
}

#endif
