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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_STABLE_PARTITION_HPP
#define RANGES_V3_ALGORITHM_STABLE_PARTITION_HPP

#include <memory>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/memory.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/partition_copy.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C, typename P = ident>
        using StablePartitionable = meta::strict_and<
            ForwardIterator<I>,
            Permutable<I>,
            IndirectPredicate<C, projected<I, P>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct stable_partition_fn
        {
        private:
            template<typename I, typename C, typename P, typename D, typename Pair>
            static I impl(I begin, I end, C pred, P proj, D len, Pair const p, concepts::ForwardIterator *fi)
            {
                // *begin is known to be false
                // len >= 1
                if(len == 1)
                    return begin;
                if(len == 2)
                {
                    I tmp = begin;
                    if(invoke(pred, invoke(proj, *++tmp)))
                    {
                        ranges::iter_swap(begin, tmp);
                        return tmp;
                    }
                    return begin;
                }
                if(len <= p.second)
                {   // The buffer is big enough to use
                    // Move the falses into the temporary buffer, and the trues to the front of the line
                    // Update begin to always point to the end of the trues
                    auto tmpbuf = make_raw_buffer(p.first);
                    auto buf = tmpbuf.begin();
                    *buf = iter_move(begin);
                    ++buf;
                    auto res = partition_copy(make_move_iterator(next(begin)),
                        make_move_sentinel(end), begin, buf, std::ref(pred), std::ref(proj));
                    // All trues now at start of range, all falses in buffer
                    // Move falses back into range, but don't mess up begin which points to first false
                    ranges::move(p.first, res.out2().base().base(), res.out1());
                    // h destructs moved-from values out of the temp buffer, but doesn't deallocate buffer
                    return res.out1();
                }
                // Else not enough buffer, do in place
                // len >= 3
                D half = len / 2;  // half >= 2
                I middle = next(begin, half);
                // recurse on [begin, middle), *begin know to be false
                // F?????????????????
                // f       m         l
                I begin_false = stable_partition_fn::impl(begin, middle, pred, proj, half, p, fi);
                // TTTFFFFF??????????
                // f  ff   m         l
                // recurse on [middle, end], except increase middle until *(middle) is false, *end know to be true
                I m1 = middle;
                D len_half = len - half;
                while(invoke(pred, invoke(proj, *m1)))
                {
                    if(++m1 == end)
                        return ranges::rotate(begin_false, middle, end).begin();
                    --len_half;
                }
                // TTTFFFFFTTTF??????
                // f  ff   m  m1     l
                I end_false = stable_partition_fn::impl(m1, end, pred, proj, len_half, p, fi);
                // TTTFFFFFTTTTTFFFFF
                // f  ff   m    sf   l
                return ranges::rotate(begin_false, middle, end_false).begin();
                // TTTTTTTTFFFFFFFFFF
                //         |
            }

            template<typename I, typename S, typename C, typename P>
            static I impl(I begin, S end, C pred, P proj, concepts::ForwardIterator *fi)
            {
                using difference_type = difference_type_t<I>;
                difference_type const alloc_limit = 3;  // might want to make this a function of trivial assignment
                // Either prove all true and return begin or point to first false
                while(true)
                {
                    if(begin == end)
                        return begin;
                    if(!invoke(pred, invoke(proj, *begin)))
                        break;
                    ++begin;
                }
                // We now have a reduced range [begin, end)
                // *begin is known to be false
                using value_type = value_type_t<I>;
                auto len_end = enumerate(begin, end);
                auto p = len_end.first >= alloc_limit ?
                    std::get_temporary_buffer<value_type>(len_end.first) : detail::value_init{};
                std::unique_ptr<value_type, detail::return_temporary_buffer> const h{p.first};
                return stable_partition_fn::impl(begin, len_end.second, pred, proj, len_end.first, p, fi);
            }

            template<typename I, typename C, typename P, typename D, typename Pair>
            static I impl(I begin, I end, C pred, P proj, D len, Pair p, concepts::BidirectionalIterator *bi)
            {
                // *begin is known to be false
                // *end is known to be true
                // len >= 2
                if(len == 2)
                {
                    ranges::iter_swap(begin, end);
                    return end;
                }
                if(len == 3)
                {
                    I tmp = begin;
                    if(invoke(pred, invoke(proj, *++tmp)))
                    {
                        ranges::iter_swap(begin, tmp);
                        ranges::iter_swap(tmp, end);
                        return end;
                    }
                    ranges::iter_swap(tmp, end);
                    ranges::iter_swap(begin, tmp);
                    return tmp;
                }
                if(len <= p.second)
                {   // The buffer is big enough to use
                    // Move the falses into the temporary buffer, and the trues to the front of the line
                    // Update begin to always point to the end of the trues
                    auto tmpbuf = ranges::make_raw_buffer(p.first);
                    auto buf = tmpbuf.begin();
                    *buf = iter_move(begin);
                    ++buf;
                    auto res = partition_copy(make_move_iterator(next(begin)),
                        make_move_sentinel(end), begin, buf, std::ref(pred), std::ref(proj));
                    begin = res.out1();
                    // move *end, known to be true
                    *begin = iter_move(res.in());
                    ++begin;
                    // All trues now at start of range, all falses in buffer
                    // Move falses back into range, but don't mess up begin which points to first false
                    move(p.first, res.out2().base().base(), begin);
                    // h destructs moved-from values out of the temp buffer, but doesn't deallocate buffer
                    return begin;
                }
                // Else not enough buffer, do in place
                // len >= 4
                I middle = begin;
                D half = len / 2;  // half >= 2
                advance(middle, half);
                // recurse on [begin, middle-1], except reduce middle-1 until *(middle-1) is true, *begin know to be false
                // F????????????????T
                // f       m        l
                I m1 = middle;
                I begin_false = begin;
                D len_half = half;
                while(!invoke(pred, invoke(proj, *--m1)))
                {
                    if(m1 == begin)
                        goto first_half_done;
                    --len_half;
                }
                // F???TFFF?????????T
                // f   m1  m        l
                begin_false = stable_partition_fn::impl(begin, m1, pred, proj, len_half, p, bi);
            first_half_done:
                // TTTFFFFF?????????T
                // f  ff   m        l
                // recurse on [middle, end], except increase middle until *(middle) is false, *end know to be true
                m1 = middle;
                len_half = len - half;
                while(invoke(pred, invoke(proj, *m1)))
                {
                    if(++m1 == end)
                        return ranges::rotate(begin_false, middle, ++end).begin();
                    --len_half;
                }
                // TTTFFFFFTTTF?????T
                // f  ff   m  m1    l
                I end_false = stable_partition_fn::impl(m1, end, pred, proj, len_half, p, bi);
                // TTTFFFFFTTTTTFFFFF
                // f  ff   m    sf  l
                return ranges::rotate(begin_false, middle, end_false).begin();
                // TTTTTTTTFFFFFFFFFF
                //         |
            }

            template<typename I, typename S, typename C, typename P>
            static I impl(I begin, S end_, C pred, P proj, concepts::BidirectionalIterator *bi)
            {
                using difference_type = difference_type_t<I>;
                using value_type = value_type_t<I>;
                difference_type const alloc_limit = 4;  // might want to make this a function of trivial assignment
                // Either prove all true and return begin or point to first false
                while(true)
                {
                    if(begin == end_)
                        return begin;
                    if(!invoke(pred, invoke(proj, *begin)))
                        break;
                    ++begin;
                }
                // begin points to first false, everything prior to begin is already set.
                // Either prove [begin, end) is all false and return begin, or point end to last true
                I end = ranges::next(begin, end_);
                do
                {
                    if(begin == --end)
                        return begin;
                } while(!invoke(pred, invoke(proj, *end)));
                // We now have a reduced range [begin, end]
                // *begin is known to be false
                // *end is known to be true
                // len >= 2
                auto len = distance(begin, end) + 1;
                auto p = len >= alloc_limit ?
                    std::get_temporary_buffer<value_type>(len) : detail::value_init{};
                std::unique_ptr<value_type, detail::return_temporary_buffer> const h{p.first};
                return stable_partition_fn::impl(begin, end, pred, proj, len, p, bi);
            }

        public:
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(StablePartitionable<I, C, P>() && Sentinel<S, I>())>
            I operator()(I begin, S end, C pred, P proj = P{}) const
            {
                return stable_partition_fn::impl(std::move(begin), std::move(end), std::ref(pred),
                    std::ref(proj), iterator_concept<I>());
            }

            // BUGBUG Can this be optimized if Rng has O1 size?
            template<typename Rng, typename C, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(StablePartitionable<I, C, P>() && Range<Rng>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `stable_partition_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<stable_partition_fn>,
                               stable_partition)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
