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
#ifndef RANGES_V3_ALGORITHM_SEARCH_N_HPP
#define RANGES_V3_ALGORITHM_SEARCH_N_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct search_n_fn
    {
    private:
        template<typename I, typename S, typename D, typename V, typename C, typename P>
        static subrange<I> sized_impl(I const begin_, S end, D const d_, D count,
            V const &val, C &pred, P &proj)
        {
            D d = d_; // always the distance from begin to end
            auto begin = uncounted(begin_);
            while(true)
            {
                // Find begin element in sequence 1 that matches val, with a mininum of loop checks
                while(true)
                {
                    if(d < count)  // return the end if we've run out of room
                    {
                        auto e = ranges::next(recounted(begin_, std::move(begin), d_ - d), std::move(end));
                        return {e, e};
                    }
                    if(invoke(pred, invoke(proj, *begin), val))
                        break;
                    ++begin;
                    --d;
                }
                // *begin matches val, now match elements after here
                auto m = begin;
                D c = 0;
                while(true)
                {
                    if(++c == count)  // If pattern exhausted, begin is the answer (works for 1 element pattern)
                        return {recounted(begin_, std::move(begin), d_ - d),
                                recounted(begin_, std::move(++m), d_ - d)};
                    ++m;  // No need to check, we know we have room to match successfully
                    if(!invoke(pred, invoke(proj, *m), val))  // if there is a mismatch, restart with a new begin
                    {
                        begin = next(std::move(m));
                        d -= (c+1);
                        break;
                    }  // else there is a match, check next elements
                }
            }
        }

        template<typename I, typename S, typename D, typename V, typename C, typename P>
        static subrange<I> impl(I begin, S end, D count, V const &val, C &pred, P &proj)
        {
            while(true)
            {
                // Find begin element in sequence 1 that matches val, with a mininum of loop checks
                while(true)
                {
                    if(begin == end)  // return end if no element matches val
                        return {begin, begin};
                    if(invoke(pred, invoke(proj, *begin), val))
                        break;
                    ++begin;
                }
                // *begin matches val, now match elements after here
                I m = begin;
                D c = 0;
                while(true)
                {
                    if(++c == count)  // If pattern exhausted, begin is the answer (works for 1 element pattern)
                        return {begin, ++m};
                    if(++m == end)  // Otherwise if source exhausted, pattern not found
                        return {m, m};
                    if(!invoke(pred, invoke(proj, *m), val))  // if there is a mismatch, restart with a new begin
                    {
                        begin = next(std::move(m));
                        break;
                    }  // else there is a match, check next elements
                }
            }
        }
    public:
        template<typename I, typename S, typename V, typename C = equal_to, typename P = identity>
        auto operator()(I begin, S end, iter_difference_t<I> count, V const &val,
                C pred = C{}, P proj = P{}) const ->
            CPP_ret(subrange<I>)(
                requires ForwardIterator<I> && Sentinel<S, I> &&
                    IndirectlyComparable<I, V const *, C, P>)
        {
            if(count <= 0)
                return {begin, begin};
            if RANGES_CONSTEXPR_IF(SizedSentinel<S, I>)
                return search_n_fn::sized_impl(std::move(begin), std::move(end),
                    distance(begin, end), count, val, pred, proj);
            else
                return search_n_fn::impl(std::move(begin), std::move(end), count, val, pred,
                    proj);
        }

        template<typename Rng, typename V, typename C = equal_to, typename P = identity>
        auto operator()(Rng &&rng, iter_difference_t<iterator_t<Rng>> count, V const &val,
                C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_subrange_t<Rng>)(
                requires ForwardRange<Rng> && IndirectlyComparable<iterator_t<Rng>, V const *, C, P>)
        {
            if(count <= 0)
                return subrange<iterator_t<Rng>>{begin(rng), begin(rng)};
            if RANGES_CONSTEXPR_IF(SizedRange<Rng>)
                return search_n_fn::sized_impl(begin(rng), end(rng), distance(rng), count, val,
                    pred, proj);
            else
                return search_n_fn::impl(begin(rng), end(rng), count, val, pred, proj);
        }
    };

    /// \sa `search_n_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(search_n_fn, search_n)

    namespace cpp20
    {
        using ranges::search_n;
    }
    /// @}
} // namespace ranges

#endif // include guard
