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
#ifndef RANGES_V3_ALGORITHM_SEARCH_HPP
#define RANGES_V3_ALGORITHM_SEARCH_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Searchable = meta::fast_and<
            ForwardIterator<I1>,
            ForwardIterator<I2>,
            Comparable<I1, I2, C, P1, P2>>;

        struct search_fn
        {
        private:
            template<typename I1, typename D1, typename I2, typename S2, typename D2,
                typename C, typename P1, typename P2,
                CONCEPT_REQUIRES_(RandomAccessIterator<I1>())>
            static I1 sized_impl(I1 const begin1_, I1 end1, D1 d1, I2 begin2, S2 end2, D2 d2,
                C &pred, P1 &proj1, P2 &proj2)
            {
                if(d1 < d2)
                    return end1;
                auto begin1 = uncounted(begin1_);
                auto const s = uncounted(end1 - (d2 - 1)); // Start of pattern match can't go beyond here
                while(true)
                {
                    // Find begin element in sequence 1 that matches *begin2, with a mininum of loop checks
                    while(true)
                    {
                        if(begin1 == s)  // return the end if we've run out of room
                            return end1;
                        if(pred(proj1(*begin1), proj2(*begin2)))
                            break;
                        ++begin1;
                    }
                    // *begin1 matches *begin2, now match elements after here
                    auto m1 = begin1;
                    I2 m2 = begin2;
                    while(true)
                    {
                        if(++m2 == end2)  // If pattern exhausted, begin1 is the answer (works for 1 element pattern)
                            return recounted(begin1_, std::move(begin1));
                        ++m1;  // No need to check, we know we have room to match successfully
                        if(!pred(proj1(*m1), proj2(*m2)))  // if there is a mismatch, restart with a new begin1
                        {
                            ++begin1;
                            break;
                        }  // else there is a match, check next elements
                    }
                }
            }

            template<typename I1, typename S1, typename D1, typename I2, typename S2, typename D2,
                typename C, typename P1, typename P2>
            static I1 sized_impl(I1 const begin1_, S1 end1, D1 const d1_, I2 begin2, S2 end2, D2 d2,
                C &pred, P1 &proj1, P2 &proj2)
            {
                D1 d1 = d1_;
                auto begin1 = uncounted(begin1_);
                while(true)
                {
                    // Find begin element in sequence 1 that matches *begin2, with a mininum of loop checks
                    while(true)
                    {
                        if(d1 < d2)  // return the end if we've run out of room
                            return next_to(recounted(begin1_, std::move(begin1), d1_ - d1), std::move(end1));
                        if(pred(proj1(*begin1), proj2(*begin2)))
                            break;
                        ++begin1;
                        --d1;
                    }
                    // *begin1 matches *begin2, now match elements after here
                    auto m1 = begin1;
                    I2 m2 = begin2;
                    while(true)
                    {
                        if(++m2 == end2)  // If pattern exhausted, begin1 is the answer (works for 1 element pattern)
                            return recounted(begin1_, std::move(begin1), d1_ - d1);
                        ++m1;  // No need to check, we know we have room to match successfully
                        if(!pred(proj1(*m1), proj2(*m2)))  // if there is a mismatch, restart with a new begin1
                        {
                            ++begin1;
                            --d1;
                            break;
                        }  // else there is a match, check next elements
                    }
                }
            }

            template<typename I1, typename S1, typename I2, typename S2, typename C, typename P1, typename P2>
            static I1 impl(I1 begin1, S1 end1, I2 begin2, S2 end2, C &pred, P1 &proj1, P2 &proj2)
            {
                while(true)
                {
                    // Find begin element in sequence 1 that matches *begin2, with a mininum of loop checks
                    while(true)
                    {
                        if(begin1 == end1)  // return end1 if no element matches *begin2
                            return begin1;
                        if(pred(proj1(*begin1), proj2(*begin2)))
                            break;
                        ++begin1;
                    }
                    // *begin1 matches *begin2, now match elements after here
                    I1 m1 = begin1;
                    I2 m2 = begin2;
                    while(true)
                    {
                        if(++m2 == end2)  // If pattern exhausted, begin1 is the answer (works for 1 element pattern)
                            return begin1;
                        if(++m1 == end1)  // Otherwise if source exhaused, pattern not found
                            return m1;
                        if(!pred(proj1(*m1), proj2(*m2)))  // if there is a mismatch, restart with a new begin1
                        {
                            ++begin1;
                            break;
                        }  // else there is a match, check next elements
                    }
                }
            }
        public:
            template<typename I1, typename S1, typename I2, typename S2,
                typename C = equal_to, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(
                    Searchable<I1, I2, C, P1, P2>() &&
                    IteratorRange<I1, S1>() &&
                    IteratorRange<I2, S2>()
                )>
            I1 operator()(I1 begin1, S1 end1, I2 begin2, S2 end2,
                C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                if(begin2 == end2)
                    return begin1;
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                if(SizedIteratorRange<I1, S1>() && SizedIteratorRange<I2, S2>())
                    return search_fn::sized_impl(std::move(begin1), std::move(end1), distance(begin1, end1),
                        std::move(begin2), std::move(end2), distance(begin2, end2), pred, proj1, proj2);
                else
                    return search_fn::impl(std::move(begin1), std::move(end1),
                        std::move(begin2), std::move(end2), pred, proj1, proj2);
            }

            template<typename Rng1, typename Rng2, typename C = equal_to, typename P1 = ident,
                typename P2 = ident, typename I1 = range_iterator_t<Rng1>, typename I2 = range_iterator_t<Rng2>,
                CONCEPT_REQUIRES_(
                    Searchable<I1, I2, C, P1, P2>() &&
                    Iterable<Rng1 &>() &&
                    Iterable<Rng2>()
                )>
            I1 operator()(Rng1 & rng1, Rng2 && rng2, C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                if(empty(rng2))
                    return begin(rng1);
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                if(SizedIterable<Rng1>() && SizedIterable<Rng2>())
                    return search_fn::sized_impl(begin(rng1), end(rng1), distance(rng1),
                        begin(rng2), end(rng2), distance(rng2), pred, proj1, proj2);
                else
                    return search_fn::impl(begin(rng1), end(rng1),
                        begin(rng2), end(rng2), pred, proj1, proj2);
            }
        };

        constexpr with_braced_init_args<search_fn> search{};

    } // namespace v3
} // namespace ranges

#endif // include guard
