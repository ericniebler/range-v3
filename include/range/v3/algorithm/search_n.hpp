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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_SEARCH_N_HPP
#define RANGES_V3_ALGORITHM_SEARCH_N_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
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
        /// ingroup group-concepts
        template<typename I, typename V, typename C = equal_to, typename P = ident>
        using Searchnable = meta::fast_and<
            ForwardIterator<I>,
            IndirectCallableRelation<C, Project<I, P>, V const *>>;

        /// \addtogroup group-algorithms
        /// @{
        struct search_n_fn
        {
        private:
            template<typename I, typename D, typename V, typename C, typename P,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            RANGES_CXX14_CONSTEXPR
            static I sized_impl(I const begin_, I end, D d, D count, V const &val,
                C &pred, P &proj)
            {
                if(d < count)
                    return end;
                auto begin = uncounted(begin_);
                auto const s = uncounted(end - (count - 1)); // Start of pattern match can't go beyond here
                while(true)
                {
                    // Find first element in sequence that matches val, with a mininum of loop checks
                    while(true)
                    {
                        if(begin >= s)  // return the end if we've run out of room
                            return end;
                        if(pred(proj(*begin), val))
                            break;
                        ++begin;
                    }
                    // *begin matches val, now match elements after here
                    auto m = begin;
                    D c = 0;
                    while(true)
                    {
                        if(++c == count)  // If pattern exhausted, begin is the answer (works for 1 element pattern)
                            return recounted(begin_, std::move(begin));
                        ++m;  // No need to check, we know we have room to match successfully
                        if(!pred(proj(*m), val))  // if there is a mismatch, restart with a new begin
                        {
                            begin = next(std::move(m));
                            break;
                        }  // else there is a match, check next elements
                    }
                }
            }

            template<typename I, typename S, typename D, typename V, typename C, typename P>
            RANGES_CXX14_CONSTEXPR
            static I sized_impl(I const begin_, S end, D const d_, D count,
                V const &val, C &pred, P &proj)
            {
                D d = d_; // always the distance from begin to end
                auto begin = uncounted(begin_);
                while(true)
                {
                    // Find begin element in sequence 1 that matches val, with a mininum of loop checks
                    while(true)
                    {
                        if(d < count)  // return the end if we've run out of room
                            return ranges::next(recounted(begin_, std::move(begin), d_ - d), std::move(end));
                        if(pred(proj(*begin), val))
                            break;
                        ++begin;
                        --d;
                    }
                    // *begin matches val, now match elements after here
                    auto m = begin;
                    D c = 0;
                    while(true)
                    {
                        if(++c == count)  // If pattern exhausted, begin is the answer (works for 1 element pattern)
                            return recounted(begin_, std::move(begin), d_ - d);
                        ++m;  // No need to check, we know we have room to match successfully
                        if(!pred(proj(*m), val))  // if there is a mismatch, restart with a new begin
                        {
                            begin = next(std::move(m));
                            d -= (c+1);
                            break;
                        }  // else there is a match, check next elements
                    }
                }
            }

            template<typename I, typename S, typename D, typename V, typename C, typename P>
            RANGES_CXX14_CONSTEXPR
            static I impl(I begin, S end, D count, V const &val, C &pred, P &proj)
            {
                while(true)
                {
                    // Find begin element in sequence 1 that matches val, with a mininum of loop checks
                    while(true)
                    {
                        if(begin == end)  // return end if no element matches val
                            return begin;
                        if(pred(proj(*begin), val))
                            break;
                        ++begin;
                    }
                    // *begin matches val, now match elements after here
                    I m = begin;
                    D c = 0;
                    while(true)
                    {
                        if(++c == count)  // If pattern exhausted, begin is the answer (works for 1 element pattern)
                            return begin;
                        if(++m == end)  // Otherwise if source exhausted, pattern not found
                            return m;
                        if(!pred(proj(*m), val))  // if there is a mismatch, restart with a new begin
                        {
                            begin = next(std::move(m));
                            break;
                        }  // else there is a match, check next elements
                    }
                }
            }
        public:
            template<typename I, typename S, typename V, typename C = equal_to, typename P = ident,
                CONCEPT_REQUIRES_(Searchnable<I, V, C, P>() && IteratorRange<I, S>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I begin, S end, iterator_difference_t<I> count, V const &val,
                C pred_ = C{}, P proj_ = P{}) const
            {
                if(count <= 0)
                    return begin;
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                if(SizedIteratorRange<I, S>())
                    return search_n_fn::sized_impl(std::move(begin), std::move(end),
                        distance(begin, end), count, val, pred, proj);
                else
                    return search_n_fn::impl(std::move(begin), std::move(end), count, val, pred,
                        proj);
            }

            template<typename Rng, typename V, typename C = equal_to, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Searchnable<I, V, C, P>() && Iterable<Rng>())>
            RANGES_CXX14_CONSTEXPR
            range_safe_iterator_t<Rng>
            operator()(Rng &&rng, iterator_difference_t<I> count, V const &val, C pred_ = C{},
                P proj_ = P{}) const
            {
                if(count <= 0)
                    return begin(rng);
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                if(SizedIterable<Rng>())
                    return search_n_fn::sized_impl(begin(rng), end(rng), distance(rng), count, val,
                        pred, proj);
                else
                    return search_n_fn::impl(begin(rng), end(rng), count, val, pred, proj);
            }
        };

        /// \sa `search_n_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& search_n = static_const<with_braced_init_args<search_n_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
