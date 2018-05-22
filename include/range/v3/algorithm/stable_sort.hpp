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

#ifndef RANGES_V3_ALGORITHM_STABLE_SORT_HPP
#define RANGES_V3_ALGORITHM_STABLE_SORT_HPP

#include <memory>
#include <iterator>
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
#include <range/v3/algorithm/merge.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/inplace_merge.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct stable_sort_fn
        {
            template<typename I, typename C, typename P>
            static void inplace_stable_sort(I begin, I end, C &pred, P &proj)
            {
                if(end - begin < 15)
                    return detail::insertion_sort(begin, end, pred, proj), void();
                I middle = begin + (end - begin) / 2;
                stable_sort_fn::inplace_stable_sort(begin, middle, pred, proj);
                stable_sort_fn::inplace_stable_sort(middle, end, pred, proj);
                detail::inplace_merge_no_buffer(begin, middle, end, middle - begin, end - middle,
                    std::ref(pred), std::ref(proj));
            }

            template<typename I1, typename I2, typename D, typename C, typename P>
            static void merge_sort_loop(I1 begin, I1 end, I2 result, D step_size, C &pred, P &proj)
            {
                D two_step = 2 * step_size;
                while(end - begin >= two_step)
                {
                    result = merge(make_move_iterator(begin),
                        make_move_iterator(begin + step_size),
                        make_move_iterator(begin + step_size),
                        make_move_iterator(begin + two_step), result,
                        std::ref(pred), std::ref(proj), std::ref(proj)).out();
                    begin += two_step;
                }
                step_size = ranges::min(D(end - begin), step_size);
                merge(make_move_iterator(begin), make_move_iterator(begin + step_size),
                    make_move_iterator(begin + step_size), make_move_iterator(end), result,
                    std::ref(pred), std::ref(proj), std::ref(proj));
            }

            static constexpr int merge_sort_chunk_size() { return 7; }

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

            // buffer points to raw memory, we create objects, and then restore the buffer to
            // raw memory by destroying the objects on return.
            template<typename I, typename V, typename C, typename P>
            static void merge_sort_with_buffer(I begin, I end, V *buffer, C &pred, P &proj)
            {
                difference_type_t<I> len = end - begin, step_size = stable_sort_fn::merge_sort_chunk_size();
                stable_sort_fn::chunk_insertion_sort(begin, end, step_size, pred, proj);
                if(step_size >= len)
                    return;
                // The first call to merge_sort_loop moves into raw storage. Construct on-demand
                // and keep track of how many objects we need to destroy.
                V *buffer_end = buffer + len;
                auto tmpbuf = make_raw_buffer(buffer);
                stable_sort_fn::merge_sort_loop(begin, end, tmpbuf.begin(), step_size, pred, proj);
                step_size *= 2;
            loop:
                stable_sort_fn::merge_sort_loop(buffer, buffer_end, begin, step_size, pred, proj);
                step_size *= 2;
                if(step_size >= len)
                    return;
                stable_sort_fn::merge_sort_loop(begin, end, buffer, step_size, pred, proj);
                step_size *= 2;
                goto loop;
            }

            // buffer points to raw memory
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
                    Sentinel<S, I>())>
            I operator()(I begin, S end_, C pred = C{}, P proj = P{}) const
            {
                I end = ranges::next(begin, end_);
                using D = difference_type_t<I>;
                using V = value_type_t<I>;
                D len = end - begin;
                auto buf = len > 256 ? std::get_temporary_buffer<V>(len) : detail::value_init{};
                std::unique_ptr<V, detail::return_temporary_buffer> h{buf.first};
                if(buf.first == nullptr)
                    stable_sort_fn::inplace_stable_sort(begin, end, pred, proj);
                else
                    stable_sort_fn::stable_sort_adaptive(begin, end, buf.first, D(buf.second), pred, proj);
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

        /// \sa `stable_sort_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<stable_sort_fn>, stable_sort)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
