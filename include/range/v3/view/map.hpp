// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_MAP_HPP
#define RANGES_V3_VIEW_MAP_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct get_first
            {
                template<typename Pair>
                auto operator()(Pair && p) const
                    -> decltype((std::forward<Pair>(p).first))
                {
                    return std::forward<Pair>(p).first;
                }
            };

            struct get_second
            {
                template<typename Pair>
                auto operator()(Pair && p) const
                    -> decltype((std::forward<Pair>(p).second))
                {
                    return std::forward<Pair>(p).second;
                }
            };
        }

        namespace view
        {
            struct map_keys : bindable<map_keys>, pipeable<map_keys>
            {
                template<typename InputRange>
                static keys_range_view<InputRange> invoke(map_keys, InputRange && rng)
                {
                    return {std::forward<InputRange>(rng), detail::get_first{}};
                }
            };

            struct map_values : bindable<map_values>, pipeable<map_values>
            {
                template<typename InputRange>
                static values_range_view<InputRange> invoke(map_values, InputRange && rng)
                {
                    return {std::forward<InputRange>(rng), detail::get_second{}};
                }
            };

            RANGES_CONSTEXPR map_keys keys {};
            RANGES_CONSTEXPR map_values values {};
        }
    }
}

#endif
