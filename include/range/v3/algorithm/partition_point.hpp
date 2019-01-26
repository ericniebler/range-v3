/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
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
#ifndef RANGES_V3_ALGORITHM_PARTITION_POINT_HPP
#define RANGES_V3_ALGORITHM_PARTITION_POINT_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/aux_/partition_point_n.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    struct partition_point_fn
    {
        template<typename I, typename S, typename C, typename P = identity>
        auto operator()(I begin, S end, C pred, P proj = P{}) const ->
            CPP_ret(I)(
                requires ForwardIterator<I> && Sentinel<S, I> &&
                    IndirectUnaryPredicate<C, projected<I, P>>)
        {
            if RANGES_CONSTEXPR_IF (SizedSentinel<S, I>)
            {
                auto len = distance(begin, std::move(end));
                return aux::partition_point_n(
                    std::move(begin), len, std::move(pred), std::move(proj));
            }

            // Probe exponentially for either end-of-range or an iterator
            // that is past the partition point (i.e., does not satisfy pred).
            auto len = iter_difference_t<I>{1};
            while(true)
            {
                auto mid = begin;
                auto d = advance(mid, len, end);
                if(mid == end || !invoke(pred, invoke(proj, *mid)))
                {
                    len -= d;
                    return aux::partition_point_n(
                        std::move(begin), len, std::ref(pred), std::ref(proj));
                }
                begin = std::move(mid);
                len *= 2;
            }
        }

        template<typename Rng, typename C, typename P = identity>
        auto operator()(Rng &&rng, C pred, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires ForwardRange<Rng> &&
                    IndirectUnaryPredicate<C, projected<iterator_t<Rng>, P>>)
        {
            if RANGES_CONSTEXPR_IF (SizedRange<Rng>)
            {
                auto len = distance(rng);
                return aux::partition_point_n(
                    begin(rng), len, std::move(pred), std::move(proj));
            }
            return (*this)(
                begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `partition_point_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(partition_point_fn, partition_point)
    /// @}
} // namespace ranges

#endif // include guard
