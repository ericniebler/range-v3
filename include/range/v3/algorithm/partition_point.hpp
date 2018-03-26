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
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/algorithm/aux_/partition_point_n.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{

        struct partition_point_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionPointable<I, C, P>() &&
                    Sentinel<S, I>() && !SizedSentinel<S, I>())>
            I operator()(I begin, S end, C pred, P proj = P{}) const
            {
                // Probe exponentially for either end-of-range or an iterator
                // that is past the partition point (i.e., does not satisfy pred).
                auto len = difference_type_t<I>{1};
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

            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionPointable<I, C, P>() &&
                    SizedSentinel<S, I>())>
            I operator()(I begin, S end, C pred, P proj = P{}) const
            {
                auto len = distance(begin, std::move(end));
                return aux::partition_point_n(
                    std::move(begin), len, std::move(pred), std::move(proj));
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Range<Rng>() && !SizedRange<Rng>() &&
                    PartitionPointable<I, C, P>())>
            safe_iterator_t<Rng> operator()(Rng && rng, C pred, P proj = P{}) const
            {
                return (*this)(
                    begin(rng), end(rng), std::move(pred), std::move(proj));
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(SizedRange<Rng>() && PartitionPointable<I, C, P>())>
            safe_iterator_t<Rng> operator()(Rng && rng, C pred, P proj = P{}) const
            {
                auto len = distance(rng);
                return aux::partition_point_n(
                    begin(rng), len, std::move(pred), std::move(proj));
            }
        };

        /// \sa `partition_point_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<partition_point_fn>, partition_point)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
