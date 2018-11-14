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
#ifndef RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP
#define RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP

#include <new>
#include <memory>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/memory.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/nullptr_v.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/algorithm/upper_bound.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/merge.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            struct merge_adaptive_fn
            {
            private:
                template<typename I, typename C, typename P>
                static void impl(I begin, I middle, I end, difference_type_t<I> len1,
                    difference_type_t<I> len2, value_type_t<I> *const buf, C &pred, P &proj)
                {
                    auto tmpbuf = make_raw_buffer(buf);
                    if(len1 <= len2)
                    {
                        auto p = ranges::move(begin, middle, tmpbuf.begin()).second;
                        merge(make_move_iterator(buf), make_move_iterator(p.base().base()),
                            make_move_iterator(std::move(middle)),
                            make_move_iterator(std::move(end)), std::move(begin),
                            std::ref(pred), std::ref(proj), std::ref(proj));
                    }
                    else
                    {
                        auto p = ranges::move(middle, end, tmpbuf.begin()).second;
                        using RBi = ranges::reverse_iterator<I>;
                        using Rv = ranges::reverse_iterator<value_type_t<I> *>;
                        merge(make_move_iterator(RBi{std::move(middle)}),
                            make_move_iterator(RBi{std::move(begin)}),
                            make_move_iterator(Rv{p.base().base()}),
                            make_move_iterator(Rv{buf}), RBi{std::move(end)},
                            not_fn(std::ref(pred)), std::ref(proj), std::ref(proj));
                    }
                }

            public:
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
                void operator()(I begin, I middle, I end, difference_type_t<I> len1,
                    difference_type_t<I> len2, value_type_t<I> *buf,
                    std::ptrdiff_t buf_size, C pred = C{}, P proj = P{}) const
                {
                    using D = difference_type_t<I>;
                    while(true)
                    {
                        // if middle == end, we're done
                        if(len2 == 0)
                            return;
                        // shrink [begin, middle) as much as possible (with no moves), returning if it shrinks to 0
                        for(; true; ++begin, --len1)
                        {
                            if(len1 == 0)
                                return;
                            if(invoke(pred, invoke(proj, *middle), invoke(proj, *begin)))
                                break;
                        }
                        if(len1 <= buf_size || len2 <= buf_size)
                        {
                            merge_adaptive_fn::impl(std::move(begin), std::move(middle),
                                std::move(end), len1, len2, buf, pred, proj);
                            return;
                        }
                        // begin < middle < end
                        // *begin > *middle
                        // partition [begin, m1) [m1, middle) [middle, m2) [m2, end) such that
                        //     all elements in:
                        //         [begin, m1)  <= [middle, m2)
                        //         [middle, m2) <  [m1, middle)
                        //         [m1, middle) <= [m2, end)
                        //     and m1 or m2 is in the middle of its range
                        I m1;  // "median" of [begin, middle)
                        I m2;  // "median" of [middle, end)
                        D len11;      // distance(begin, m1)
                        D len21;      // distance(middle, m2)
                        // binary search smaller range
                        if(len1 < len2)
                        {   // len >= 1, len2 >= 2
                            len21 = len2 / 2;
                            m2 = next(middle, len21);
                            m1 = upper_bound(begin, middle, invoke(proj, *m2), std::ref(pred), std::ref(proj));
                            len11 = distance(begin, m1);
                        }
                        else
                        {
                            if(len1 == 1)
                            {   // len1 >= len2 && len2 > 0, therefore len2 == 1
                                // It is known *begin > *middle
                                ranges::iter_swap(begin, middle);
                                return;
                            }
                            // len1 >= 2, len2 >= 1
                            len11 = len1 / 2;
                            m1 = next(begin, len11);
                            m2 = lower_bound(middle, end, invoke(proj, *m1), std::ref(pred), std::ref(proj));
                            len21 = distance(middle, m2);
                        }
                        D len12 = len1 - len11;  // distance(m1, middle)
                        D len22 = len2 - len21;  // distance(m2, end)
                        // [begin, m1) [m1, middle) [middle, m2) [m2, end)
                        // swap middle two partitions
                        middle = rotate(m1, std::move(middle), m2).begin();
                        // len12 and len21 now have swapped meanings
                        // merge smaller range with recursive call and larger with tail recursion elimination
                        if(len11 + len21 < len12 + len22)
                        {
                            (*this)(std::move(begin), std::move(m1), middle, len11, len21, buf, buf_size,
                                std::ref(pred), std::ref(proj));
                            begin = std::move(middle);
                            middle = std::move(m2);
                            len1 = len12;
                            len2 = len22;
                        }
                        else
                        {
                            (*this)(middle, std::move(m2), std::move(end), len12, len22, buf, buf_size,
                                std::ref(pred), std::ref(proj));
                            end = std::move(middle);
                            middle = std::move(m1);
                            len1 = len11;
                            len2 = len21;
                        }
                    }
                }
            };

            RANGES_INLINE_VARIABLE(merge_adaptive_fn, merge_adaptive)

            struct inplace_merge_no_buffer_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
                void operator()(I begin, I middle, I end, difference_type_t<I> len1,
                    difference_type_t<I> len2, C pred = C{}, P proj = P{}) const
                {
                    merge_adaptive(std::move(begin), std::move(middle), std::move(end), len1, len2,
                        _nullptr_v<value_type_t<I>>(), 0, std::move(pred), std::move(proj));
                }
            };

            RANGES_INLINE_VARIABLE(inplace_merge_no_buffer_fn,
                                   inplace_merge_no_buffer)
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct inplace_merge_fn
        {
            // TODO reimplement to only need forward iterators
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
            I operator()(I begin, I middle, S end, C pred = C{}, P proj = P{}) const
            {
                using value_type = value_type_t<I>;
                auto len1 = distance(begin, middle);
                auto len2_and_end = enumerate(middle, end);
                auto buf_size = ranges::min(len1, len2_and_end.first);
                std::pair<value_type*, std::ptrdiff_t> buf{nullptr, 0};
                std::unique_ptr<value_type, detail::return_temporary_buffer> h;
                if(detail::is_trivially_copy_assignable<value_type>::value && 8 < buf_size)
                {
                    buf = detail::get_temporary_buffer<value_type>(buf_size);
                    h.reset(buf.first);
                }
                detail::merge_adaptive(std::move(begin), std::move(middle), len2_and_end.second,
                    len1, len2_and_end.first, buf.first, buf.second, std::move(pred),
                    std::move(proj));
                return len2_and_end.second;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && Sortable<I, C, P>())>
            safe_iterator_t<Rng>
            operator()(Rng &&rng, I middle, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `inplace_merge_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<inplace_merge_fn>,
                               inplace_merge)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
