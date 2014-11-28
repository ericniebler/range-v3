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

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename C, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using PartitionPointable = meta::fast_and<
            ForwardIterator<I>,
            Invokable<P, V>,
            InvokablePredicate<C, X>>;

        // BUGBUG should partition_point also return the end iterator?
        struct partition_point_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionPointable<I, C, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, C pred_, P proj_ = P{}) const
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
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
                CONCEPT_REQUIRES_(PartitionPointable<I, C, P>() && Iterable<Rng &>())>
            I operator()(Rng &rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        constexpr partition_point_fn partition_point{};

    } // namespace v3
} // namespace ranges

#endif // include guard
