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

#ifndef RANGES_V3_VIEW_MAP_HPP
#define RANGES_V3_VIEW_MAP_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/pipeable.hpp>

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
            struct keys_fn : pipeable<keys_fn>
            {
                template<typename Rng>
                keys_range_view<Rng> operator()(Rng && rng) const
                {
                    return {std::forward<Rng>(rng), detail::get_first{}};
                }
            };

            struct values_fn : pipeable<values_fn>
            {
                template<typename Rng>
                values_range_view<Rng> operator()(Rng && rng) const
                {
                    return {std::forward<Rng>(rng), detail::get_second{}};
                }
            };

            constexpr keys_fn keys {};
            constexpr values_fn values {};
        }
    }
}

#endif
