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
                template<typename Rng,
                    CONCEPT_REQUIRES_(Function<Action, Rng &>() &&
                        (Range<Rng>() || (Container<Rng>() && !std::is_reference<Rng>())))>
                detail::uncvref_t<Rng> operator()(Rng && rng) const
                {
                    Action{}(rng);
                    return std::forward<Rng>(rng);
                }
            };

            template<typename Rng, typename Action,
                typename RngRef = detail::conditional_t<(Range<Rng>()), Rng &, reference_wrapper<Rng>>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && Function<Action, RngRef>() && is_pipeable<Action>())>
            Rng & operator|=(Rng & rng, Action && action)
            {
                RngRef{rng} | action;
                return rng;
            }
        }
    }
}

#endif
