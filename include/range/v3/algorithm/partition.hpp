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
#ifndef RANGES_V3_ALGORITHM_PARTITION_HPP
#define RANGES_V3_ALGORITHM_PARTITION_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename C, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using Partitionable = meta::fast_and<
            ForwardIterator<I>,
            Permutable<I>,
            Invokable<P, V>,
            InvokablePredicate<C, X>>;

        struct partition_fn
        {
        private:
            template<typename I, typename S, typename C, typename P>
            static I impl(I begin, S end, C pred_, P proj_, concepts::ForwardIterator*)
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
                while(true)
                {
                    if(begin == end)
                        return begin;
                    if(!pred(proj(*begin)))
                        break;
                    ++begin;
                }
                for(I p = begin; ++p != end;)
                {
                    if(pred(proj(*p)))
                    {
                        ranges::iter_swap(begin, p);
                        ++begin;
                    }
                }
                return begin;
            }

            template<typename I, typename S, typename C, typename P>
            static I impl(I begin, S end_, C pred_, P proj_, concepts::BidirectionalIterator*)
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
                I end = next_to(begin, end_);
                while(true)
                {
                    while(true)
                    {
                        if(begin == end)
                            return begin;
                        if(!pred(proj(*begin)))
                            break;
                        ++begin;
                    }
                    do
                    {
                        if(begin == --end)
                            return begin;
                    } while(!pred(proj(*end)));
                    ranges::iter_swap(begin, end);
                    ++begin;
                }
            }
        public:
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(Partitionable<I, C, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, C pred, P proj = P{}) const
            {
                return partition_fn::impl(std::move(begin), std::move(end), std::move(pred),
                    std::move(proj), iterator_concept<I>());
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Partitionable<I, C, P>() && Iterable<Rng &>())>
            I operator()(Rng &rng, C pred, P proj = P{}) const
            {
                return partition_fn::impl(begin(rng), end(rng), std::move(pred),
                    std::move(proj), iterator_concept<I>());
            }
        };

        constexpr partition_fn partition{};

    } // namespace v3
} // namespace ranges

#endif // include guard
