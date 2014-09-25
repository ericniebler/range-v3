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
#ifndef RANGES_V3_ALGORITHM_SORT_HPP
#define RANGES_V3_ALGORITHM_SORT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        // TODO implement Introsort
        // TODO Forward iterators, like EoP?
        struct sort_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessIterator<I>() &&
                    IteratorRange<I, S>())> // BUGBUG could be Forward
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return sort_heap(begin, make_heap(begin, end, pred, proj), pred, proj);
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessIterable<Rng>())> // BUGBUG could be Forward
            I operator()(Rng & rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR sort_fn sort {};

    } // namespace v3
} // namespace ranges

#endif // include guard
