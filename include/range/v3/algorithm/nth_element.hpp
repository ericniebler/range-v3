/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef RANGES_V3_ALGORITHM_NTH_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_NTH_ELEMENT_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            // stable, 2-3 compares, 0-2 swaps

            template<typename I, typename C, typename P,
                typename V = iterator_common_reference_t<I>,
                typename X = concepts::Function::result_t<P, V>,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && Function<P, V>() && Relation<C, X>())>
            unsigned sort3(I x, I y, I z, C &pred, P &proj)
            {
                unsigned r = 0;
                if(!pred(proj(*y), proj(*x)))           // if x <= y
                {
                    if(!pred(proj(*z), proj(*y)))       // if y <= z
                        return r;                       // x <= y && y <= z
                                                        // x <= y && y > z
                    ranges::iter_swap(y, z);            // x <= z && y < z
                    r = 1;
                    if(pred(proj(*y), proj(*x)))        // if x > y
                    {
                        ranges::iter_swap(x, y);        // x < y && y <= z
                        r = 2;
                    }
                    return r;                           // x <= y && y < z
                }
                if(pred(proj(*z), proj(*y)))            // x > y, if y > z
                {
                    ranges::iter_swap(x, z);            // x < y && y < z
                    r = 1;
                    return r;
                }
                ranges::iter_swap(x, y);                // x > y && y <= z
                r = 1;                                  // x < y && x <= z
                if(pred(proj(*z), proj(*y)))            // if y > z
                {
                    ranges::iter_swap(y, z);            // x <= y && y < z
                    r = 2;
                }
                return r;
            }                                           // x <= y && y <= z

            // Assumes size > 0
            template<typename I, typename C, typename P,
                typename V = iterator_common_reference_t<I>,
                typename X = concepts::Function::result_t<P, V>,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Function<P, V>() && Relation<C, X>())>
            void selection_sort(I begin, I end, C &pred, P &proj)
            {
                RANGES_ASSERT(begin != end);
                for(I lm1 = ranges::prev(end); begin != lm1; ++begin)
                {
                    I i = ranges::min_element(begin, end, std::ref(pred), std::ref(proj));
                    if(i != begin)
                        ranges::iter_swap(begin, i);
                }
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct nth_element_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sortable<I, C, P>())>
            I operator()(I begin, I nth, S end_, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                I end = ranges::next(nth, end_), end_orig = end;
                // C is known to be a reference type
                using difference_type = iterator_difference_t<I>;
                difference_type const limit = 7;
                while(true)
                {
                restart:
                    if(nth == end)
                        return end_orig;
                    difference_type len = end - begin;
                    switch(len)
                    {
                    case 0:
                    case 1:
                        return end_orig;
                    case 2:
                        if(pred(proj(*--end), proj(*begin)))
                            ranges::iter_swap(begin, end);
                        return end_orig;
                    case 3:
                        {
                        I m = begin;
                        detail::sort3(begin, ++m, --end, pred, proj);
                        return end_orig;
                        }
                    }
                    if(len <= limit)
                    {
                        detail::selection_sort(begin, end, pred, proj);
                        return end_orig;
                    }
                    // len > limit >= 3
                    I m = begin + len/2;
                    I lm1 = end;
                    unsigned n_swaps = detail::sort3(begin, m, --lm1, pred, proj);
                    // *m is median
                    // partition [begin, m) < *m and *m <= [m, end)
                    //(this inhibits tossing elements equivalent to m around unnecessarily)
                    I i = begin;
                    I j = lm1;
                    // j points beyond range to be tested, *lm1 is known to be <= *m
                    // The search going up is known to be guarded but the search coming down isn't.
                    // Prime the downward search with a guard.
                    if(!pred(proj(*i), proj(*m)))  // if *begin == *m
                    {
                        // *begin == *m, *begin doesn't go in begin part
                        // manually guard downward moving j against i
                        while(true)
                        {
                            if(i == --j)
                            {
                                // *begin == *m, *m <= all other elements
                                // Parition instead into [begin, i) == *begin and *begin < [i, end)
                                ++i;  // begin + 1
                                j = end;
                                if(!pred(proj(*begin), proj(*--j)))  // we need a guard if *begin == *(end-1)
                                {
                                    while(true)
                                    {
                                        if(i == j)
                                            return end_orig;  // [begin, end) all equivalent elements
                                        if(pred(proj(*begin), proj(*i)))
                                        {
                                            ranges::iter_swap(i, j);
                                            ++n_swaps;
                                            ++i;
                                            break;
                                        }
                                        ++i;
                                    }
                                }
                                // [begin, i) == *begin and *begin < [j, end) and j == end - 1
                                if(i == j)
                                    return end_orig;
                                while(true)
                                {
                                    while(!pred(proj(*begin), proj(*i)))
                                        ++i;
                                    while(pred(proj(*begin), proj(*--j)))
                                        ;
                                    if(i >= j)
                                        break;
                                    ranges::iter_swap(i, j);
                                    ++n_swaps;
                                    ++i;
                                }
                                // [begin, i) == *begin and *begin < [i, end)
                                // The begin part is sorted,
                                if(nth < i)
                                    return end_orig;
                                // nth_element the second part
                                // nth_element<C>(i, nth, end, pred);
                                begin = i;
                                goto restart;
                            }
                            if(pred(proj(*j), proj(*m)))
                            {
                                ranges::iter_swap(i, j);
                                ++n_swaps;
                                break;  // found guard for downward moving j, now use unguarded partition
                            }
                        }
                    }
                    ++i;
                    // j points beyond range to be tested, *lm1 is known to be <= *m
                    // if not yet partitioned...
                    if(i < j)
                    {
                        // known that *(i - 1) < *m
                        while(true)
                        {
                            // m still guards upward moving i
                            while(pred(proj(*i), proj(*m)))
                                ++i;
                            // It is now known that a guard exists for downward moving j
                            while(!pred(proj(*--j), proj(*m)))
                                ;
                            if(i >= j)
                                break;
                            ranges::iter_swap(i, j);
                            ++n_swaps;
                            // It is known that m != j
                            // If m just moved, follow it
                            if(m == i)
                                m = j;
                            ++i;
                        }
                    }
                    // [begin, i) < *m and *m <= [i, end)
                    if(i != m && pred(proj(*m), proj(*i)))
                    {
                        ranges::iter_swap(i, m);
                        ++n_swaps;
                    }
                    // [begin, i) < *i and *i <= [i+1, end)
                    if(nth == i)
                        return end_orig;
                    if(n_swaps == 0)
                    {
                        // We were given a perfectly partitioned sequence.  Coincidence?
                        if(nth < i)
                        {
                            // Check for [begin, i) already sorted
                            j = m = begin;
                            while(++j != i)
                            {
                                if(pred(proj(*j), proj(*m)))
                                    // not yet sorted, so sort
                                    goto not_sorted;
                                m = j;
                            }
                            // [begin, i) sorted
                            return end_orig;
                        }
                        else
                        {
                            // Check for [i, end) already sorted
                            j = m = i;
                            while(++j != end)
                            {
                                if(pred(proj(*j), proj(*m)))
                                    // not yet sorted, so sort
                                    goto not_sorted;
                                m = j;
                            }
                            // [i, end) sorted
                            return end_orig;
                        }
                    }
            not_sorted:
                    // nth_element on range containing nth
                    if(nth < i)
                    {
                        // nth_element<C>(begin, nth, i, pred);
                        end = i;
                    }
                    else
                    {
                        // nth_element<C>(i+1, nth, end, pred);
                        begin = ++i;
                    }
                }
                return end_orig;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    RandomAccessRange<Rng>() &&
                    Sortable<I, C, P>()
                )>
            range_safe_iterator_t<Rng>
            operator()(Rng &&rng, I nth, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), std::move(nth), end(rng), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `nth_element_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<nth_element_fn>, nth_element)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
