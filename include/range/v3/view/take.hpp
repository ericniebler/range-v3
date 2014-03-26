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
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Iterator>
            using iterator_size_t =
                typename std::make_unsigned<iterator_difference_t<Iterator>>::type;
        }

        namespace view
        {
            struct taker : bindable<taker>
            {
                template<typename InputIterable,
                    CONCEPT_REQUIRES_(ranges::Iterable<InputIterable>() &&
                                      ranges::InputIterator<range_iterator_t<InputIterable>>())>
                static sliced_view<InputIterable>
                invoke(taker, InputIterable && rng, range_size_t<InputIterable> to)
                {
                    return {std::forward<InputIterable>(rng), 0, to};
                }
                template<typename InputIterator,
                    CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>())>
                static counted_view<InputIterator>
                invoke(taker, InputIterator it, detail::iterator_size_t<InputIterator> n)
                {
                    return {std::move(it),
                            static_cast<iterator_difference_t<InputIterator>>(n)};
                }
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto
                invoke(taker take, Int to) ->
                    decltype(take.move_bind(std::placeholders::_1, (Int)to))
                {
                    return take.move_bind(std::placeholders::_1, (Int)to);
                }
            };

            RANGES_CONSTEXPR taker take {};
        }
    }
}

#endif
