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

#include <range/v3/algorithm/aux_/partition_point_n.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    RANGES_BEGIN_NIEBLOID(partition_point)

        /// \brief function template \c partition_point
        template<typename I, typename S, typename C, typename P = identity>
        auto RANGES_FUN_NIEBLOID(partition_point)(
            I first, S last, C pred, P proj = P{}) //
            ->CPP_ret(I)(                          //
                requires forward_iterator<I> && sentinel_for<S, I> &&
                indirect_unary_predicate<C, projected<I, P>>)
        {
            if(RANGES_CONSTEXPR_IF(sized_sentinel_for<S, I>))
            {
                auto len = distance(first, std::move(last));
                return aux::partition_point_n(
                    std::move(first), len, std::move(pred), std::move(proj));
            }

            // Probe exponentially for either last-of-range or an iterator
            // that is past the partition point (i.e., does not satisfy pred).
            auto len = iter_difference_t<I>{1};
            while(true)
            {
                auto mid = first;
                auto d = advance(mid, len, last);
                if(mid == last || !invoke(pred, invoke(proj, *mid)))
                {
                    len -= d;
                    return aux::partition_point_n(
                        std::move(first), len, std::ref(pred), std::ref(proj));
                }
                first = std::move(mid);
                len *= 2;
            }
        }

        /// \overload
        template<typename Rng, typename C, typename P = identity>
        auto RANGES_FUN_NIEBLOID(partition_point)(Rng && rng, C pred, P proj = P{}) //
            ->CPP_ret(safe_iterator_t<Rng>)(                                        //
                requires forward_range<Rng> &&
                indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        {
            if(RANGES_CONSTEXPR_IF(sized_range<Rng>))
            {
                auto len = distance(rng);
                return aux::partition_point_n(
                    begin(rng), len, std::move(pred), std::move(proj));
            }
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(partition_point)

    namespace cpp20
    {
        using ranges::partition_point;
    }
    /// @}
} // namespace ranges

#endif // include guard
