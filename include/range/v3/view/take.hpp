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

#ifndef RANGES_V3_VIEW_TAKE_HPP
#define RANGES_V3_VIEW_TAKE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            struct taker : bindable<taker>
            {
                template<typename InputIterable,
                    CONCEPT_REQUIRES_(ranges::InputIterable<InputIterable>())>
                static slice_range_view<InputIterable>
                invoke(taker, InputIterable && rng, std::size_t to)
                {
                    return {std::forward<InputIterable>(rng), 0, to};
                }
                template<typename InputIterator,
                    CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>())>
                static counted_iterable_view<InputIterator>
                invoke(taker, InputIterator it, std::size_t n)
                {
                    return {std::move(it),
                            static_cast<iterator_difference_t<InputIterator>>(n)};
                }
                static auto
                invoke(taker take, std::size_t to) ->
                    decltype(take.move_bind(std::placeholders::_1, (std::size_t)to))
                {
                    return take.move_bind(std::placeholders::_1, (std::size_t)to);
                }
            };

            RANGES_CONSTEXPR taker take {};
        }
    }
}

#endif
