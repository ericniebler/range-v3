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
#ifndef RANGES_V3_ALGORITHM_PARTITION_HPP
#define RANGES_V3_ALGORITHM_PARTITION_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        CPP_def
        (
            template(typename I, typename C, typename P = ident)
            (concept Partitionable)(I, C, P),
                ForwardIterator<I> &&
                Permutable<I> &&
                IndirectPredicate<C, projected<I, P>>
        );

        /// \addtogroup group-algorithms
        /// @{
        struct partition_fn
        {
        private:
            template<typename I, typename S, typename C, typename P>
            static I impl(I begin, S end, C pred, P proj, forward_iterator_tag)
            {
                while(true)
                {
                    if(begin == end)
                        return begin;
                    if(!invoke(pred, invoke(proj, *begin)))
                        break;
                    ++begin;
                }
                for(I p = begin; ++p != end;)
                {
                    if(invoke(pred, invoke(proj, *p)))
                    {
                        ranges::iter_swap(begin, p);
                        ++begin;
                    }
                }
                return begin;
            }

            template<typename I, typename S, typename C, typename P>
            static I impl(I begin, S end_, C pred, P proj, bidirectional_iterator_tag)
            {
                I end = ranges::next(begin, end_);
                while(true)
                {
                    while(true)
                    {
                        if(begin == end)
                            return begin;
                        if(!invoke(pred, invoke(proj, *begin)))
                            break;
                        ++begin;
                    }
                    do
                    {
                        if(begin == --end)
                            return begin;
                    } while(!invoke(pred, invoke(proj, *end)));
                    ranges::iter_swap(begin, end);
                    ++begin;
                }
            }
        public:
            template<typename I, typename S, typename C, typename P = ident>
            auto operator()(I begin, S end, C pred, P proj = P{}) const ->
                CPP_ret(I)(
                    requires Partitionable<I, C, P> && Sentinel<S, I>)
            {
                return partition_fn::impl(std::move(begin), std::move(end), std::move(pred),
                    std::move(proj), iterator_tag_of<I>());
            }

            template<typename Rng, typename C, typename P = ident>
            auto operator()(Rng &&rng, C pred, P proj = P{}) const ->
                CPP_ret(safe_iterator_t<Rng>)(
                    requires Partitionable<iterator_t<Rng>, C, P> && Range<Rng>)
            {
                return partition_fn::impl(begin(rng), end(rng), std::move(pred),
                    std::move(proj), iterator_tag_of<iterator_t<Rng>>());
            }
        };

        /// \sa `partition_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<partition_fn>, partition)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
