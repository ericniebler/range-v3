/// \file
// Range v3 library
//
// Copyright Eric Niebler 2004
// Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define RANGES_V3_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        template <typename I, typename O, typename BOp = minus, typename P = ident,
                  typename V = iterator_value_t<I>,
                  typename X = concepts::Callable::result_t<P, V>,
                  typename Y = concepts::Callable::result_t<BOp, X, X>>
        using AdjacentDifferentiable = meta::strict_and<
            InputIterator<I>,
            OutputIterator<O, X &&>,
            OutputIterator<O, Y &&>,
            Callable<P, V>,
            Callable<BOp, X, X>,
            CopyConstructible<uncvref_t<X>>,
            Movable<uncvref_t<X>>>;

        struct adjacent_difference_fn
        {
            template <typename I, typename S, typename O, typename BOp = minus,
                      typename P = ident,
                      CONCEPT_REQUIRES_(Sentinel<S, I>() &&
                                        AdjacentDifferentiable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(I begin, S end, O result, BOp bop_ = BOp{}, P proj_ = P{}) const
            {
                // BUGBUG think about the use of coerce here.
                auto &&bop = as_function(bop_);
                auto &&proj = as_function(proj_);
                using V = iterator_value_t<I>;
                using X = concepts::Callable::result_t<P, V>;
                coerce<V> v;
                coerce<X> x;

                if(begin != end)
                {
                    auto t1(x(proj(v(*begin))));
                    *result = t1;
                    for(++begin, ++result; begin != end; ++begin, ++result)
                    {
                        auto t2(x(proj(v(*begin))));
                        *result = bop(t2, t1);
                        t1 = std::move(t2);
                    }
                }
                return {begin, result};
            }

            template <typename I, typename S, typename O, typename S2,
                      typename BOp = minus, typename P = ident,
                      CONCEPT_REQUIRES_(Sentinel<S, I>() && Sentinel<S2, O>() &&
                                        AdjacentDifferentiable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(I begin, S end, O result, S2 end_result, BOp bop_ = BOp{},
                       P proj_ = P{}) const
            {
                auto &&bop = as_function(bop_);
                auto &&proj = as_function(proj_);
                using V = iterator_value_t<I>;
                using X = concepts::Callable::result_t<P, V>;
                coerce<V> v;
                coerce<X> x;

                if(begin != end && result != end_result)
                {
                    auto t1(x(proj(v(*begin))));
                    *result = t1;
                    for(++begin, ++result; begin != end && result != end_result;
                        ++begin, ++result)
                    {
                        auto t2(x(proj(v(*begin))));
                        *result = bop(t2, t1);
                        t1 = std::move(t2);
                    }
                }
                return {begin, result};
            }

            template <typename Rng, typename ORef, typename BOp = minus, typename P = ident,
                      typename I = range_iterator_t<Rng>, typename O = uncvref_t<ORef>,
                      CONCEPT_REQUIRES_(Range<Rng &>() &&
                                        AdjacentDifferentiable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(Rng &rng, ORef &&result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::forward<ORef>(result), std::move(bop),
                               std::move(proj));
            }

            template <typename Rng, typename ORng, typename BOp = minus,
                      typename P = ident, typename I = range_iterator_t<Rng>,
                      typename O = range_iterator_t<ORng>,
                      CONCEPT_REQUIRES_(Range<Rng &>() && Range<ORng &>() &&
                                        AdjacentDifferentiable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(Rng &rng, ORng &result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), begin(result), end(result),
                               std::move(bop), std::move(proj));
            }
        };

        RANGES_INLINE_VARIABLE(adjacent_difference_fn, adjacent_difference)
    }
}

#endif
