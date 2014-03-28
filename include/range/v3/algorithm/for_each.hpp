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
        struct for_each_fun
        {
            template<typename InputIterator, typename Sentinel, typename Fun,
                CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>() &&
                                  ranges::EqualityComparable<InputIterator, Sentinel>() &&
                                  ranges::Callable<Fun, iterator_reference_t<InputIterator>>())>
            InputIterator
            operator()(InputIterator begin, Sentinel end, Fun fun) const
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
            operator()(InputIterable &rng, Fun fun) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), fun);
            }
        };

        RANGES_CONSTEXPR for_each_fun for_each{};
    } // namespace v3
} // namespace ranges

#endif // include guard
