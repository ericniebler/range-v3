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
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct partition_fn
    {
    private:
        template<typename I, typename S, typename C, typename P>
        static I impl(I first, S last, C pred, P proj, detail::forward_iterator_tag_)
        {
            while(true)
            {
                if(first == last)
                    return first;
                if(!invoke(pred, invoke(proj, *first)))
                    break;
                ++first;
            }
            for(I p = first; ++p != last;)
            {
                if(invoke(pred, invoke(proj, *p)))
                {
                    ranges::iter_swap(first, p);
                    ++first;
                }
            }
            return first;
        }

        template<typename I, typename S, typename C, typename P>
        static I impl(I first, S end_, C pred, P proj,
                      detail::bidirectional_iterator_tag_)
        {
            I last = ranges::next(first, end_);
            while(true)
            {
                while(true)
                {
                    if(first == last)
                        return first;
                    if(!invoke(pred, invoke(proj, *first)))
                        break;
                    ++first;
                }
                do
                {
                    if(first == --last)
                        return first;
                } while(!invoke(pred, invoke(proj, *last)));
                ranges::iter_swap(first, last);
                ++first;
            }
        }

    public:
        template<typename I, typename S, typename C, typename P = identity>
        auto operator()(I first, S last, C pred, P proj = P{}) const -> CPP_ret(I)( //
            requires permutable<I> && sentinel_for<S, I> &&
                indirect_unary_predicate<C, projected<I, P>>)
        {
            return partition_fn::impl(std::move(first),
                                      std::move(last),
                                      std::move(pred),
                                      std::move(proj),
                                      iterator_tag_of<I>());
        }

        template<typename Rng, typename C, typename P = identity>
        auto operator()(Rng && rng, C pred, P proj = P{}) const
            -> CPP_ret(safe_iterator_t<Rng>)( //
                requires forward_range<Rng> && permutable<iterator_t<Rng>> &&
                    indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        {
            return partition_fn::impl(begin(rng),
                                      end(rng),
                                      std::move(pred),
                                      std::move(proj),
                                      iterator_tag_of<iterator_t<Rng>>());
        }
    };

    /// \sa `partition_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(partition_fn, partition)

    namespace cpp20
    {
        using ranges::partition;
    }
    /// @}
} // namespace ranges

#endif // include guard
