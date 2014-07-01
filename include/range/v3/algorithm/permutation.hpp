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
#ifndef RANGES_V3_ALGORITHM_PERMUTATION_HPP
#define RANGES_V3_ALGORITHM_PERMUTATION_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/algorithm/reverse.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct next_permutation_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(BidirectionalIterator<I, S>() && Sortable<I, C, P>())>
            bool operator()(I begin, S end_, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                if(begin == end_)
                    return false;
                I end = advance_to(begin, end_), i = end;
                if(begin == --i)
                    return false;
                while(true)
                {
                    I ip1 = i;
                    if(pred(proj(*--i), proj(*ip1)))
                    {
                        I j = end;
                        while(!pred(proj(*i), proj(*--j)))
                            ;
                        ranges::swap(*i, *j);
                        ranges::reverse(ip1, end);
                        return true;
                    }
                    if(i == begin)
                    {
                        ranges::reverse(begin, end);
                        return false;
                    }
                }
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(BidirectionalIterable<Rng>() && Sortable<I, C, P>())>
            bool operator()(Rng &rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR next_permutation_fn next_permutation{};

        struct prev_permutation_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(BidirectionalIterator<I, S>() && Sortable<I, C, P>())>
            bool operator()(I begin, S end_, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                if(begin == end_)
                    return false;
                I end = advance_to(begin, end_), i = end;
                if(begin == --i)
                    return false;
                while(true)
                {
                    I ip1 = i;
                    if(pred(proj(*ip1), proj(*--i)))
                    {
                        I j = end;
                        while(!pred(proj(*--j), proj(*i)))
                            ;
                        ranges::swap(*i, *j);
                        ranges::reverse(ip1, end);
                        return true;
                    }
                    if(i == begin)
                    {
                        ranges::reverse(begin, end);
                        return false;
                    }
                }
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(BidirectionalIterable<Rng>() && Sortable<I, C, P>())>
            bool operator()(Rng &rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR prev_permutation_fn prev_permutation{};

    } // namespace v3
} // namespace ranges

#endif // include guard
