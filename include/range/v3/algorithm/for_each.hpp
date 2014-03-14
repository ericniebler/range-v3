//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterator, typename Sentinel, typename Fun,
            CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>() &&
                              ranges::EqualityComparable<InputIterator, Sentinel>() &&
                              ranges::Callable<Fun, iterator_reference_t<InputIterator>>())>
        InputIterator
        for_each(InputIterator begin, Sentinel end, Fun fun)
        {
            for(; begin != end; ++begin)
            {
                fun(*begin);
            }
            return begin;
        }

        template<typename InputIterable, typename Fun,
            CONCEPT_REQUIRES_(ranges::Iterable<InputIterable>() &&
                              ranges::InputIterator<range_iterator_t<InputIterable>>() &&
                              ranges::Callable<Fun, range_reference_t<InputIterable>>())>
        range_iterator_t<InputIterable>
        for_each(InputIterable &rng, Fun fun)
        {
            return ranges::for_each(ranges::begin(rng), ranges::end(rng), fun);
        }

    } // namespace v3
} // namespace ranges

#endif // include guard
