/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
//  Copyright (c) 1994
//  Hewlett-Packard Company
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Hewlett-Packard Company makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
//
//  Copyright (c) 1996
//  Silicon Graphics Computer Systems, Inc.
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Silicon Graphics makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
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
#include <range/v3/algorithm/move_backward.hpp>
#include <range/v3/algorithm/partial_sort.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I, typename C, typename P>
            inline I unguarded_partition(I begin, I end, C &pred, P &proj)
            {
                I mid = begin + (end - begin) / 2, last = ranges::prev(end);
                auto &&x = *begin, &&y = *mid, &&z = *last;
                auto &&a = invoke(proj, (decltype(x) &&)x), &&b = invoke(proj, (decltype(y) &&)y), &&c = invoke(proj, (decltype(z) &&)z);

                // Find the median:
                I pivot_pnt = invoke(pred, a, b)
                  ? (invoke(pred, b, c) ? mid   : (invoke(pred, a, c) ? last : begin))
                  : (invoke(pred, a, c) ? begin : (invoke(pred, b, c) ? last : mid  ));

                // Do the partition:
                while(true)
                {
                    auto &&v = *pivot_pnt;
                    auto &&pivot = invoke(proj, (decltype(v) &&)v);
                    while(invoke(pred, invoke(proj, *begin), pivot))
                        ++begin;
                    --end;
                    while(invoke(pred, pivot, invoke(proj, *end)))
                        --end;
                    if(!(begin < end))
                        return begin;
                    ranges::iter_swap(begin, end);
                    pivot_pnt = pivot_pnt == begin ? end : (pivot_pnt == end ? begin : pivot_pnt);
                    ++begin;
                }
            }

            template<typename I, typename C, typename P>
            inline void unguarded_linear_insert(I end, value_type_t<I> val, C &pred, P &proj)
            {
                I next = prev(end);
                while(invoke(pred, invoke(proj, val), invoke(proj, *next)))
                {
                    *end = iter_move(next);
                    end = next;
                    --next;
                }
                *end = std::move(val);
            }

            template<typename I, typename C, typename P>
            inline void linear_insert(I begin, I end, C &pred, P &proj)
            {
                value_type_t<I> val = iter_move(end);
                if(invoke(pred, invoke(proj, val), invoke(proj, *begin)))
                {
                    move_backward(begin, end, end + 1);
                    *begin = std::move(val);
                }
                else
                    detail::unguarded_linear_insert(end, std::move(val), pred, proj);
            }

            template<typename I, typename C, typename P>
            inline void insertion_sort(I begin, I end, C &pred, P &proj)
            {
                if(begin == end)
                    return;
                for(I i = next(begin); i != end; ++i)
                    detail::linear_insert(begin, i, pred, proj);
            }

            template<typename I, typename C, typename P>
            inline void unguarded_insertion_sort(I begin, I end, C &pred, P &proj)
            {
                for(I i = begin; i != end; ++i)
                    detail::unguarded_linear_insert(i, iter_move(i), pred, proj);
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{

        // Introsort: Quicksort to a certain depth, then Heapsort. Insertion
        // sort below a certain threshold.
        // TODO Forward iterators, like EoP?
        struct sort_fn
        {
        private:
            static constexpr int introsort_threshold() { return 16; }

            template<typename I, typename C, typename P>
            static void final_insertion_sort(I begin, I end, C &pred, P &proj)
            {
                if(end - begin > sort_fn::introsort_threshold())
                {
                    detail::insertion_sort(begin, begin + sort_fn::introsort_threshold(), pred, proj);
                    detail::unguarded_insertion_sort(begin + sort_fn::introsort_threshold(), end, pred, proj);
                }
                else
                    detail::insertion_sort(begin, end, pred, proj);
            }

            template<typename Size>
            static Size log2(Size n)
            {
                Size k = 0;
                for(; n != 1; n >>= 1)
                    ++k;
                return k;
            }

            template<typename I, typename Size, typename C, typename P>
            static void introsort_loop(I begin, I end, Size depth_limit, C &pred, P &proj)
            {
                while(end - begin > sort_fn::introsort_threshold())
                {
                    if(depth_limit == 0)
                        return partial_sort(begin, end, end, std::ref(pred), std::ref(proj)), void();
                    I cut = detail::unguarded_partition(begin, end, pred, proj);
                    sort_fn::introsort_loop(cut, end, --depth_limit, pred, proj);
                    end = cut;
                }
            }

        public:
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessIterator<I>() &&
                    Sentinel<S, I>())>
            I operator()(I begin, S end_, C pred = C{}, P proj = P{}) const
            {
                I end = ranges::next(begin, std::move(end_));
                if(begin != end)
                {
                    sort_fn::introsort_loop(begin, end, sort_fn::log2(end - begin) * 2, pred, proj);
                    sort_fn::final_insertion_sort(begin, end, pred, proj);
                }
                return end;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessRange<Rng>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `sort_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<sort_fn>, sort)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
