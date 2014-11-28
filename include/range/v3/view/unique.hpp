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

#ifndef RANGES_V3_VIEW_UNIQUE_HPP
#define RANGES_V3_VIEW_UNIQUE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/utility/pipeable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct not_equal_to
            {
                template<typename T>
                bool operator()(T && t, T && u) const
                {
                    return !(std::forward<T>(t) == std::forward<T>(u));
                }
            };
        }

        namespace view
        {
            struct unique_fn : pipeable<unique_fn>
            {
                template<typename Rng>
                uniqued_view<Rng> operator()(Rng && rng) const
                {
                    return {std::forward<Rng>(rng), detail::not_equal_to{}};
                }
            };

            constexpr unique_fn unique {};
        }
    }
}

#endif
