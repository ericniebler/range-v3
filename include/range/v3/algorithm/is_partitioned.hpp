//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_IS_PARTITIONED_HPP
#define RANGES_V3_ALGORITHM_IS_PARTITIONED_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename C, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using IsPartitionedable = logical_and_t<
            InputIterator<I>,
            Invokable<P, V>,
            InvokablePredicate<C, X>>;

        struct is_partitioned_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(IsPartitionedable<I, C, P>() && IteratorRange<I, S>())>
            bool operator()(I begin, S end, C pred_, P proj_ = P{}) const
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
                for(; begin != end; ++begin)
                    if(!pred(proj(*begin)))
                        break;
                for(; begin != end; ++begin)
                    if(pred(proj(*begin)))
                        return false;
                return true;
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsPartitionedable<I, C, P>() && Iterable<Rng>())>
            bool operator()(Rng &&rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<is_partitioned_fn> is_partitioned{};

    } // namespace v3
} // namespace ranges

#endif // include guard
