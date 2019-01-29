/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        // stable, 2-3 compares, 0-2 swaps

        template<typename I, typename C, typename P>
        auto sort3(I x, I y, I z, C &pred, P &proj) ->
            CPP_ret(unsigned)(
                requires ForwardIterator<I> && IndirectRelation<C, projected<I, P>>)
        {
            unsigned r = 0;
            if(!invoke(pred, invoke(proj, *y), invoke(proj, *x)))     // if x <= y
            {
                if(!invoke(pred, invoke(proj, *z), invoke(proj, *y))) // if y <= z
                    return r;                                         // x <= y && y <= z
                                                                      // x <= y && y > z
                ranges::iter_swap(y, z);                              // x <= z && y < z
                r = 1;
                if(invoke(pred, invoke(proj, *y), invoke(proj, *x)))  // if x > y
                {
                    ranges::iter_swap(x, y);                          // x < y && y <= z
                    r = 2;
                }
                return r;                                             // x <= y && y < z
            }
            if(invoke(pred, invoke(proj, *z), invoke(proj, *y)))      // x > y, if y > z
            {
                ranges::iter_swap(x, z);                              // x < y && y < z
                r = 1;
                return r;
            }
            ranges::iter_swap(x, y);                                  // x > y && y <= z
            r = 1;                                                    // x < y && x <= z
            if(invoke(pred, invoke(proj, *z), invoke(proj, *y)))      // if y > z
            {
                ranges::iter_swap(y, z);                              // x <= y && y < z
                r = 2;
            }
            return r;
        }                                                             // x <= y && y <= z

        template<typename I, typename C, typename P>
        auto selection_sort(I begin, I end, C &pred, P &proj) ->
            CPP_ret(void)(
                requires BidirectionalIterator<I> && IndirectRelation<C, projected<I, P>>)
        {
            RANGES_EXPECT(begin != end);
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
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, I nth, S end_, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sortable<I, C, P>)
        {
            I end = ranges::next(nth, end_), end_orig = end;
            // C is known to be a reference type
            using difference_type = iter_difference_t<I>;
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
                    if(invoke(pred, invoke(proj, *--end), invoke(proj, *begin)))
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
                if(!invoke(pred, invoke(proj, *i), invoke(proj, *m)))  // if *begin == *m
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
                            if(!invoke(pred, invoke(proj, *begin), invoke(proj, *--j)))  // we need a guard if *begin == *(end-1)
                            {
                                while(true)
                                {
                                    if(i == j)
                                        return end_orig;  // [begin, end) all equivalent elements
                                    if(invoke(pred, invoke(proj, *begin), invoke(proj, *i)))
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
                                while(!invoke(pred, invoke(proj, *begin), invoke(proj, *i)))
                                    ++i;
                                while(invoke(pred, invoke(proj, *begin), invoke(proj, *--j)))
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
                        if(invoke(pred, invoke(proj, *j), invoke(proj, *m)))
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
                        while(invoke(pred, invoke(proj, *i), invoke(proj, *m)))
                            ++i;
                        // It is now known that a guard exists for downward moving j
                        while(!invoke(pred, invoke(proj, *--j), invoke(proj, *m)))
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
                if(i != m && invoke(pred, invoke(proj, *m), invoke(proj, *i)))
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
                            if(invoke(pred, invoke(proj, *j), invoke(proj, *m)))
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
                            if(invoke(pred, invoke(proj, *j), invoke(proj, *m)))
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

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, iterator_t<Rng> nth, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng> && Sortable<iterator_t<Rng>, C, P>)
        {
            return (*this)(begin(rng), std::move(nth), end(rng), std::move(pred),
                std::move(proj));
        }
    };

    /// \sa `nth_element_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(nth_element_fn, nth_element)
    /// @}
} // namespace ranges

#endif // include guard
