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

#ifndef RANGES_V3_VIEW_UNIQUE_HPP
#define RANGES_V3_VIEW_UNIQUE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/utility/bindable.hpp>

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
            struct uniquer : bindable<uniquer>, pipeable<uniquer>
            {
                template<typename Rng>
                static uniqued_view<Rng>
                invoke(uniquer, Rng && rng)
                {
                    return {std::forward<Rng>(rng), detail::not_equal_to{}};
                }
            };

            RANGES_CONSTEXPR uniquer unique {};
        }
    }
}

#endif
