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

#ifndef RANGES_V3_ALGORITHM_STABLE_SORT_HPP
#define RANGES_V3_ALGORITHM_STABLE_SORT_HPP

#include <memory>
#include <iterator>
#include <algorithm>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/memory.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/algorithm/merge.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/inplace_merge.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct stable_sort_fn
        {
            template<typename I, typename C, typename P>
            static void inplace_stable_sort(I begin, I end, C &pred, P &proj)
            {
                if(end - begin < 15)
                {
                    detail::insertion_sort(begin, end, pred, proj);
                    return;
                }
                I middle = begin + (end - begin) / 2;
                stable_sort_fn::inplace_stable_sort(begin, middle, pred, proj);
                stable_sort_fn::inplace_stable_sort(middle, end, pred, proj);
                detail::inplace_merge_no_buffer(begin, middle, end, middle - begin, end - middle,
                    std::ref(pred), std::ref(proj));
            }

            template<typename I1, typename I2, typename D, typename C, typename P>
            static void merge_sort_loop(I1 begin, I1 end, I2 result, D step_size, C &pred, P &proj)
            {
                // BUGBUG we have moved objects out of the buffer but we haven't run destructors yet.
                using MI = std::move_iterator<I1>;
                // TODO make_counted_raw_storage_iterator and make_destroy_n
                //using RI = ranges::raw_storage_iterator<I2, iterator_value_t<I2>>;
                //using CRI = counted_iterator<RI, detail::int_ref<D>>;
                D two_step = 2 * step_size;
                //D count = 0;
                //CRI result{RI{result_}, count};
                //RI result{result_};
                while(end - begin >= two_step)
                {
                    result = std::get<2>(merge(MI{begin}, MI{begin + step_size}, MI{begin + step_size},
                        MI{begin + two_step}, result, std::ref(pred), std::ref(proj), std::ref(proj)));
                    begin += two_step;
                }
                step_size = std::min(D(end - begin), step_size);
                merge(MI{begin}, MI{begin + step_size}, MI{begin + step_size}, MI{end}, result,
                    std::ref(pred), std::ref(proj), std::ref(proj));
            }

            static constexpr int merge_sort_chunk_size = 7;

            template<typename I, typename D, typename C, typename P>
            static void chunk_insertion_sort(I begin, I end, D chunk_size, C &pred, P &proj)
            {
                while(end - begin >= chunk_size)
                {
                    detail::insertion_sort(begin, begin + chunk_size, pred, proj);
                    begin += chunk_size;
                }
                detail::insertion_sort(begin, end, pred, proj);
            }

            template<typename I, typename V, typename C, typename P>
            static void merge_sort_with_buffer(I begin, I end, V *buffer, C &pred, P &proj)
            {
                using D = iterator_difference_t<I>;
                D len = end - begin;
                V *buffer_last = buffer + len;
                D step_size = stable_sort_fn::merge_sort_chunk_size;
                stable_sort_fn::chunk_insertion_sort(begin, end, step_size, pred, proj);
                detail::destroy_n<V> d{};
                std::unique_ptr<V, detail::destroy_n<V>&> h{buffer, d};
                auto raw_buffer = make_counted_raw_storage_iterator(buffer, d);
                if(step_size < len)
                {
                    // First time through, we're moving into raw storage. Construct on-demand and
                    // keep track of how many objects we need to destroy.
                    stable_sort_fn::merge_sort_loop(begin, end, raw_buffer, step_size, pred, proj);
                    step_size *= 2;
                    stable_sort_fn::merge_sort_loop(buffer, buffer_last, begin, step_size, pred, proj);
                    step_size *= 2;
                    while(step_size < len)
                    {
                        stable_sort_fn::merge_sort_loop(begin, end, buffer, step_size, pred, proj);
                        step_size *= 2;
                        stable_sort_fn::merge_sort_loop(buffer, buffer_last, begin, step_size, pred, proj);
                        step_size *= 2;
                    }
                }
            }

            template<typename I, typename V, typename D, typename C, typename P>
            static void stable_sort_adaptive(I begin, I end, V *buffer, D buffer_size, C &pred, P &proj)
            {
                D len = (end - begin + 1) / 2;
                I middle = begin + len;
                if(len > buffer_size)
                {
                    stable_sort_fn::stable_sort_adaptive(begin, middle, buffer, buffer_size, pred, proj);
                    stable_sort_fn::stable_sort_adaptive(middle, end, buffer, buffer_size, pred, proj);
                }
                else
                {
                    stable_sort_fn::merge_sort_with_buffer(begin, middle, buffer, pred, proj);
                    stable_sort_fn::merge_sort_with_buffer(middle, end, buffer, pred, proj);
                }
                detail::merge_adaptive(begin, middle, end, middle - begin, end - middle,
                    buffer, buffer_size, std::ref(pred), std::ref(proj));
            }

        public:
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessIterator<I>() &&
                    IteratorRange<I, S>())>
            I operator()(I begin, S end_, C pred_ = C{}, P proj_ = P{}) const
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
                I end = next_to(begin, end_);
                using D = iterator_difference_t<I>;
                using V = iterator_value_t<I>;
                D len = end - begin;
                auto buf = len > 256 ? std::get_temporary_buffer<V>(end - begin) : detail::value_init{};
                std::unique_ptr<V, detail::return_temporary_buffer> h{buf.first};
                if(buf.first == nullptr)
                    stable_sort_fn::inplace_stable_sort(begin, end, pred, proj);
                else
                    stable_sort_fn::stable_sort_adaptive(begin, end, buf.first, D(buf.second), pred, proj);
                return end;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessIterable<Rng>())>
            I operator()(Rng & rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        constexpr int stable_sort_fn::merge_sort_chunk_size;

        RANGES_CONSTEXPR stable_sort_fn stable_sort {};

    } // namespace v3
} // namespace ranges

#endif // include guard
