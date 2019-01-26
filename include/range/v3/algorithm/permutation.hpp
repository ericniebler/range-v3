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
#ifndef RANGES_V3_ALGORITHM_PERMUTATION_HPP
#define RANGES_V3_ALGORITHM_PERMUTATION_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct is_permutation_fn
    {
    private:
        template<typename I1, typename S1, typename I2, typename S2, typename C, typename P1,
            typename P2>
        static bool four_iter_impl(I1 begin1, S1 end1, I2 begin2, S2 end2, C pred, P1 proj1,
            P2 proj2)
        {
            // shorten sequences as much as possible by lopping off any equal parts
            for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                    goto not_done;
            return begin1 == end1 && begin2 == end2;
        not_done:
            // begin1 != end1 && begin2 != end2 && *begin1 != *begin2
            auto l1 = distance(begin1, end1);
            auto l2 = distance(begin2, end2);
            if(l1 != l2)
                return false;

            // For each element in [f1, l1) see if there are the same number of
            //    equal elements in [f2, l2)
            for(I1 i = begin1; i != end1; ++i)
            {
                // Have we already counted the number of *i in [f1, l1)?
                for(I1 j = begin1; j != i; ++j)
                    if(invoke(pred, invoke(proj1, *j), invoke(proj1, *i)))
                        goto next_iter;
                {
                    // Count number of *i in [f2, l2)
                    iter_difference_t<I2> c2 = 0;
                    for(I2 j = begin2; j != end2; ++j)
                        if(invoke(pred, invoke(proj1, *i), invoke(proj2, *j)))
                            ++c2;
                    if(c2 == 0)
                        return false;
                    // Count number of *i in [i, l1) (we can start with 1)
                    iter_difference_t<I1> c1 = 1;
                    for(I1 j = next(i); j != end1; ++j)
                        if(invoke(pred, invoke(proj1, *i), invoke(proj1, *j)))
                            ++c1;
                    if(c1 != c2)
                        return false;
                }
        next_iter:;
            }
            return true;
        }

    public:
        template<typename I1, typename S1, typename I2, typename C = equal_to,
            typename P1 = identity, typename P2 = identity>
        RANGES_DEPRECATED("Use the variant of ranges::is_permutation that takes an upper bound "
            "for both sequences")
        auto operator()(I1 begin1, S1 end1, I2 begin2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(bool)(
                requires ForwardIterator<I1> && Sentinel<S1, I1> && ForwardIterator<I2> &&
                    IndirectlyComparable<I1, I2, C, P1, P2>)
        {
            // shorten sequences as much as possible by lopping off any equal parts
            for(; begin1 != end1; ++begin1, ++begin2)
                if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                    goto not_done;
            return true;
        not_done:
            // begin1 != end1 && *begin1 != *begin2
            auto l1 = distance(begin1, end1);
            if(l1 == 1)
                return false;
            I2 end2 = next(begin2, l1);
            // For each element in [f1, l1) see if there are the same number of
            //    equal elements in [f2, l2)
            for(I1 i = begin1; i != end1; ++i)
            {
                // Have we already counted the number of *i in [f1, l1)?
                for(I1 j = begin1; j != i; ++j)
                    if(invoke(pred, invoke(proj1, *j), invoke(proj1, *i)))
                        goto next_iter;
                {
                    // Count number of *i in [f2, l2)
                    iter_difference_t<I2> c2 = 0;
                    for(I2 j = begin2; j != end2; ++j)
                        if(invoke(pred, invoke(proj1, *i), invoke(proj2, *j)))
                            ++c2;
                    if(c2 == 0)
                        return false;
                    // Count number of *i in [i, l1) (we can start with 1)
                    iter_difference_t<I1> c1 = 1;
                    for(I1 j = next(i); j != end1; ++j)
                        if(invoke(pred, invoke(proj1, *i), invoke(proj1, *j)))
                            ++c1;
                    if(c1 != c2)
                        return false;
                }
        next_iter:;
            }
            return true;
        }

        template<typename I1, typename S1, typename I2, typename S2, typename C = equal_to,
            typename P1 = identity, typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(bool)(
                requires ForwardIterator<I1> && Sentinel<S1, I1> &&
                    ForwardIterator<I2> && Sentinel<S2, I2> &&
                    IndirectlyComparable<I1, I2, C, P1, P2>)
        {
            if RANGES_CONSTEXPR_IF (SizedSentinel<S1, I1> && SizedSentinel<S2, I2>)
            {
                RANGES_DIAGNOSTIC_PUSH
                RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
                return distance(begin1, end1) == distance(begin2, end2) &&
                    (*this)(std::move(begin1), std::move(end1), std::move(begin2),
                        std::move(pred), std::move(proj1), std::move(proj2));
                RANGES_DIAGNOSTIC_POP
            }
            return is_permutation_fn::four_iter_impl(std::move(begin1), std::move(end1),
                std::move(begin2), std::move(end2), std::move(pred), std::move(proj1),
                std::move(proj2));
        }

        template<typename Rng1, typename I2Ref, typename C = equal_to, typename P1 = identity,
            typename P2 = identity>
        RANGES_DEPRECATED("Use the variant of ranges::is_permutation that takes an upper bound "
            "for both sequences")
        auto operator()(Rng1 &&rng1, I2Ref &&begin2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(bool)(
                requires ForwardRange<Rng1> && ForwardIterator<uncvref_t<I2Ref>> &&
                    IndirectlyComparable<iterator_t<Rng1>, uncvref_t<I2Ref>, C, P1, P2>)
        {
            RANGES_DIAGNOSTIC_PUSH
            RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
            return (*this)(begin(rng1), end(rng1), (I2Ref &&) begin2, std::move(pred),
                std::move(proj1), std::move(proj2));
            RANGES_DIAGNOSTIC_POP
        }

        template<typename Rng1, typename Rng2, typename C = equal_to, typename P1 = identity,
            typename P2 = identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(bool)(
                requires ForwardRange<Rng1> && ForwardRange<Rng2> &&
                    IndirectlyComparable<iterator_t<Rng1>, iterator_t<Rng2>, C, P1, P2>)
        {
            if RANGES_CONSTEXPR_IF (SizedRange<Rng1> && SizedRange<Rng2>)
            {
                RANGES_DIAGNOSTIC_PUSH
                RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
                return distance(rng1) == distance(rng2) &&
                    (*this)(begin(rng1), end(rng1), begin(rng2), std::move(pred),
                        std::move(proj1), std::move(proj2));
                RANGES_DIAGNOSTIC_POP
            }
            return is_permutation_fn::four_iter_impl(begin(rng1), end(rng1), begin(rng2),
                end(rng2), std::move(pred), std::move(proj1), std::move(proj2));
        }
    };

    /// \sa `is_permutation_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(is_permutation_fn, is_permutation)

    struct next_permutation_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end_, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires BidirectionalIterator<I> && Sentinel<S, I> && Sortable<I, C, P>)
        {
            if(begin == end_)
                return false;
            I end = ranges::next(begin, end_), i = end;
            if(begin == --i)
                return false;
            while(true)
            {
                I ip1 = i;
                if(invoke(pred, invoke(proj, *--i), invoke(proj, *ip1)))
                {
                    I j = end;
                    while(!invoke(pred, invoke(proj, *i), invoke(proj, *--j)))
                        ;
                    ranges::iter_swap(i, j);
                    ranges::reverse(ip1, end);
                    return true;
                }
                if(i == begin)
                {
                    ranges::reverse(begin, end);
                    return false;
                }
            }
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires BidirectionalRange<Rng> && Sortable<iterator_t<Rng>, C, P>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `next_permutation_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(next_permutation_fn, next_permutation)

    struct prev_permutation_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end_, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires BidirectionalIterator<I> && Sentinel<S, I> && Sortable<I, C, P>)
        {
            if(begin == end_)
                return false;
            I end = ranges::next(begin, end_), i = end;
            if(begin == --i)
                return false;
            while(true)
            {
                I ip1 = i;
                if(invoke(pred, invoke(proj, *ip1), invoke(proj, *--i)))
                {
                    I j = end;
                    while(!invoke(pred, invoke(proj, *--j), invoke(proj, *i)))
                        ;
                    ranges::iter_swap(i, j);
                    ranges::reverse(ip1, end);
                    return true;
                }
                if(i == begin)
                {
                    ranges::reverse(begin, end);
                    return false;
                }
            }
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires BidirectionalRange<Rng> && Sortable<iterator_t<Rng>, C, P>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `prev_permutation_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(prev_permutation_fn, prev_permutation)
    /// @}
} // namespace ranges

#endif // include guard
