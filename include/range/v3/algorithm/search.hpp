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
#ifndef RANGES_V3_ALGORITHM_SEARCH_HPP
#define RANGES_V3_ALGORITHM_SEARCH_HPP

#include <meta/meta.hpp>
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
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Searchable = meta::strict_and<
            ForwardIterator<I1>,
            ForwardIterator<I2>,
            Comparable<I1, I2, C, P1, P2>>;

        /// \addtogroup group-algorithms
        /// @{
        struct search_fn
        {
        private:
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
                            return ranges::next(recounted(begin1_, std::move(begin1), d1_ - d1), std::move(end1));
                        if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
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
                        if(!invoke(pred, invoke(proj1, *m1), invoke(proj2, *m2)))  // if there is a mismatch, restart with a new begin1
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
                        if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
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
                        if(++m1 == end1)  // Otherwise if source exhausted, pattern not found
                            return m1;
                        if(!invoke(pred, invoke(proj1, *m1), invoke(proj2, *m2)))  // if there is a mismatch, restart with a new begin1
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
                    Sentinel<S1, I1>() &&
                    Sentinel<S2, I2>()
                )>
            I1 operator()(I1 begin1, S1 end1, I2 begin2, S2 end2,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                if(begin2 == end2)
                    return begin1;
                if(SizedSentinel<S1, I1>() && SizedSentinel<S2, I2>())
                    return search_fn::sized_impl(std::move(begin1), std::move(end1),
                        distance(begin1, end1), std::move(begin2), std::move(end2),
                        distance(begin2, end2), pred, proj1, proj2);
                else
                    return search_fn::impl(std::move(begin1), std::move(end1),
                        std::move(begin2), std::move(end2), pred, proj1, proj2);
            }

            template<typename Rng1, typename Rng2, typename C = equal_to, typename P1 = ident,
                typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(
                    Searchable<I1, I2, C, P1, P2>() &&
                    Range<Rng1>() &&
                    Range<Rng2>()
                )>
            safe_iterator_t<Rng1>
            operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const
            {
                if(empty(rng2))
                    return begin(rng1);
                if(SizedRange<Rng1>() && SizedRange<Rng2>())
                    return search_fn::sized_impl(begin(rng1), end(rng1), distance(rng1),
                        begin(rng2), end(rng2), distance(rng2), pred, proj1, proj2);
                else
                    return search_fn::impl(begin(rng1), end(rng1),
                        begin(rng2), end(rng2), pred, proj1, proj2);
            }
        };

        /// \sa `search_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<search_fn>, search)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
