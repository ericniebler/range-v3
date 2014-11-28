// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_NUMERIC_PARTIAL_SUM_HPP
#define RANGES_V3_NUMERIC_PARTIAL_SUM_HPP

#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template <typename I, typename O, typename BOp = plus, typename P = ident,
                  typename V = iterator_value_t<I>,
                  typename X = concepts::Invokable::result_t<P, V>,
                  typename Y = concepts::Invokable::result_t<BOp, X, X>>
        using PartialSummable = meta::fast_and<
            InputIterator<I>,
            WeakOutputIterator<O, X>,
            Invokable<P, V>,
            CopyConstructible<X>,
            Invokable<BOp, X, X>,
            Assignable<X &, Y>>;

        struct partial_sum_fn
        {
            template <typename I, typename S, typename O, typename BOp = plus,
                      typename P = ident,
                      CONCEPT_REQUIRES_(IteratorRange<I, S>() &&
                                        PartialSummable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(I begin, S end, O result, BOp bop_ = BOp{},
                       P proj_ = P{}) const
            {
                auto &&bop = invokable(bop_);
                auto &&proj = invokable(proj_);
                using V = iterator_value_t<I>;
                using X = concepts::Invokable::result_t<P, V>;
                coerce<V> v;
                coerce<X> x;

                if(begin != end)
                {
                    auto t(x(proj(v(*begin))));
                    *result = t;
                    for(++begin, ++result; begin != end; ++begin, ++result)
                    {
                        t = bop(t, proj(*begin));
                        *result = t;
                    }
                }
                return {begin, result};
            }

            template <typename I, typename S, typename O, typename S2,
                      typename BOp = plus, typename P = ident,
                      CONCEPT_REQUIRES_(IteratorRange<I, S>() && IteratorRange<O, S2>() &&
                                        PartialSummable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(I begin, S end, O result, S2 end_result, BOp bop_ = BOp{},
                       P proj_ = P{}) const
            {
                auto &&bop = invokable(bop_);
                auto &&proj = invokable(proj_);
                using V = iterator_value_t<I>;
                using X = concepts::Invokable::result_t<P, V>;
                coerce<V> v;
                coerce<X> x;

                if(begin != end && result != end_result)
                {
                    auto t(x(proj(v(*begin))));
                    *result = t;
                    for(++begin, ++result; begin != end && result != end_result;
                        ++begin, ++result)
                    {
                        t = bop(t, proj(*begin));
                        *result = t;
                    }
                }
                return {begin, result};
            }

            template <typename Rng, typename ORef, typename BOp = plus,
                      typename P = ident, typename I = range_iterator_t<Rng>,
                      typename O = uncvref_t<ORef>,
                      CONCEPT_REQUIRES_(Iterable<Rng &>() &&
                                        PartialSummable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(Rng &rng, ORef &&result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::forward<ORef>(result),
                               std::move(bop), std::move(proj));
            }

            template <typename Rng, typename ORng, typename BOp = plus,
                      typename P = ident, typename I = range_iterator_t<Rng>,
                      typename O = range_iterator_t<ORng>,
                      CONCEPT_REQUIRES_(Iterable<Rng &>() && Iterable<ORng &>() &&
                                        PartialSummable<I, O, BOp, P>())>
            std::pair<I, O>
            operator()(Rng &rng, ORng &result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), begin(result), end(result),
                               std::move(bop), std::move(proj));
            }
        };

        constexpr partial_sum_fn partial_sum{};
    }
}

#endif
