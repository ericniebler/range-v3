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
#ifndef RANGES_V3_ALGORITHM_ROTATE_HPP
#define RANGES_V3_ALGORITHM_ROTATE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/move_backward.hpp>
#include <range/v3/algorithm/swap_ranges.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct rotate_fn
        {
        private:
            template<typename I> // Forward
            static iterator_range<I> rotate_left(I begin, I end)
            {
                value_type_t<I> tmp = iter_move(begin);
                I lm1 = move(next(begin), end, begin).second;
                *lm1 = std::move(tmp);
                return {lm1, end};
            }

            template<typename I> // Bidirectional
            static iterator_range<I> rotate_right(I begin, I end)
            {
                I lm1 = prev(end);
                value_type_t<I> tmp = iter_move(lm1);
                I fp1 = move_backward(begin, lm1, end).second;
                *begin = std::move(tmp);
                return {fp1, end};
            }

            template<typename I, typename S> // Forward
            static iterator_range<I> rotate_forward(I begin, I middle, S end)
            {
                I i = middle;
                while(true)
                {
                    ranges::iter_swap(begin, i);
                    ++begin;
                    if(++i == end)
                        break;
                    if(begin == middle)
                        middle = i;
                }
                I r = begin;
                if(begin != middle)
                {
                    I j = middle;
                    while(true)
                    {
                        ranges::iter_swap(begin, j);
                        ++begin;
                        if(++j == end)
                        {
                            if(begin == middle)
                                break;
                            j = middle;
                        }
                        else if(begin == middle)
                            middle = j;
                    }
                }
                return {r, i};
            }

            template<typename D>
            static D gcd(D x, D y)
            {
                do
                {
                    D t = x % y;
                    x = y;
                    y = t;
                } while(y);
                return x;
            }

            template<typename I> // Random
            static iterator_range<I> rotate_gcd(I begin, I middle, I end)
            {
                auto const m1 = middle - begin;
                auto const m2 = end - middle;
                if(m1 == m2)
                {
                    swap_ranges(begin, middle, middle);
                    return {middle, end};
                }
                auto const g = rotate_fn::gcd(m1, m2);
                for(I p = begin + g; p != begin;)
                {
                    value_type_t<I> t = iter_move(--p);
                    I p1 = p;
                    I p2 = p1 + m1;
                    do
                    {
                        *p1 = iter_move(p2);
                        p1 = p2;
                        auto const d = end - p2;
                        if(m1 < d)
                            p2 += m1;
                        else
                            p2 = begin + (m1 - d);
                    } while(p2 != p);
                    *p1 = std::move(t);
                }
                return {begin + m2, end};
            }

            template<typename I, typename S>
            static iterator_range<I> rotate_(I begin, I middle, S end, concepts::ForwardIterator*)
            {
                return rotate_fn::rotate_forward(begin, middle, end);
            }

            template<typename I>
            static iterator_range<I> rotate_(I begin, I middle, I end, concepts::ForwardIterator*)
            {
                using value_type = value_type_t<I>;
                if(detail::is_trivially_move_assignable<value_type>::value)
                {
                    if(next(begin) == middle)
                        return rotate_fn::rotate_left(begin, end);
                }
                return rotate_fn::rotate_forward(begin, middle, end);
            }

            template<typename I>
            static iterator_range<I> rotate_(I begin, I middle, I end, concepts::BidirectionalIterator*)
            {
                using value_type = value_type_t<I>;
                if(detail::is_trivially_move_assignable<value_type>::value)
                {
                    if(next(begin) == middle)
                        return rotate_fn::rotate_left(begin, end);
                    if(next(middle) == end)
                        return rotate_fn::rotate_right(begin, end);
                }
                return rotate_fn::rotate_forward(begin, middle, end);
            }

            template<typename I>
            static iterator_range<I> rotate_(I begin, I middle, I end, concepts::RandomAccessIterator*)
            {
                using value_type = value_type_t<I>;
                if(detail::is_trivially_move_assignable<value_type>::value)
                {
                    if(next(begin) == middle)
                        return rotate_fn::rotate_left(begin, end);
                    if(next(middle) == end)
                        return rotate_fn::rotate_right(begin, end);
                    return rotate_fn::rotate_gcd(begin, middle, end);
                }
                return rotate_fn::rotate_forward(begin, middle, end);
            }

        public:
            template<typename I, typename S,
                CONCEPT_REQUIRES_(Permutable<I>() && Sentinel<S, I>())>
            iterator_range<I> operator()(I begin, I middle, S end) const
            {
                if(begin == middle)
                {
                    begin = ranges::next(std::move(begin), end);
                    return {begin, begin};
                }
                if(middle == end)
                {
                    return {begin, middle};
                }
                return rotate_fn::rotate_(begin, middle, end, iterator_concept<I>{});
            }

            template<typename Rng, typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Range<Rng>() && Permutable<I>())>
            meta::if_<std::is_lvalue_reference<Rng>, iterator_range<I>, dangling<iterator_range<I>>>
            operator()(Rng &&rng, I middle) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng));
            }
        };

        /// \sa `rotate_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<rotate_fn>, rotate)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
