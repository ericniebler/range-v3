/// \file
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
        template<typename I, typename C, typename P = ident,
            typename V = iterator_common_reference_t<I>,
            typename X = concepts::Callable::result_t<P, V>>
        using PartitionPointable = meta::strict_and<
            ForwardIterator<I>,
            IndirectCallablePredicate<C, Projected<I, P>>>;

        /// \addtogroup group-algorithms
        /// @{

        struct partition_point_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionPointable<I, C, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, C pred_, P proj_ = P{}) const
            {
                auto && pred = as_function(pred_);
                auto && proj = as_function(proj_);
                auto len = distance(begin, end);
                while(len != 0)
                {
                    auto const half = len / 2;
                    auto middle = next(uncounted(begin), half);
                    if(pred(proj(*middle)))
                    {
                        begin = recounted(begin, std::move(++middle), half + 1);
                        len -= half + 1;
                    }
                    else
                        len = half;
                }
                return begin;
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(PartitionPointable<I, C, P>() && Range<Rng>())>
            range_safe_iterator_t<Rng> operator()(Rng &&rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `partition_point_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& partition_point = static_const<with_braced_init_args<partition_point_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
