/// \file
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

#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/algorithm/tagspec.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename O, typename BOp = plus, typename P = ident,
            typename X = projected<projected<I, coerce<value_type_t<I>>>, P>,
            typename Y = reference_t<X>>
        using PartialSummable = meta::strict_and<
            SemiRegular<detail::decay_t<Y>>,
            InputIterator<I>,
            OutputIterator<O, detail::decay_t<Y> const &>,
            IndirectRegularInvocable<BOp, detail::decay_t<Y>*, X>,
            Assignable<detail::decay_t<Y>&, indirect_result_of_t<BOp&(detail::decay_t<Y>*, X)>>>;

        struct partial_sum_fn
        {
            template<typename I, typename S1, typename O, typename S2,
                typename BOp = plus, typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S1, I>() && Sentinel<S2, O>() &&
                    PartialSummable<I, O, BOp, P>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S1 end, O result, S2 end_result, BOp bop = BOp{}, P proj = P{}) const
            {
                using X = projected<projected<I, coerce<value_type_t<I>>>, P>;
                using Y = reference_t<X>;
                coerce<value_type_t<I>> val;
                if(begin != end && result != end_result)
                {
                    detail::decay_t<Y> t(invoke(proj, val(*begin)));
                    *result = t;
                    for(++begin, ++result; begin != end && result != end_result;
                        ++begin, ++result)
                    {
                        t = invoke(bop, t, invoke(proj, val(*begin)));
                        *result = t;
                    }
                }
                return {begin, result};
            }

            template<typename I, typename S, typename O, typename BOp = plus,
                typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S, I>() && PartialSummable<I, O, BOp, P>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(std::move(begin), std::move(end), std::move(result), 
                               unreachable{}, std::move(bop), std::move(proj));
            }

            template<typename Rng, typename ORef, typename BOp = plus,
                typename P = ident, typename I = iterator_t<Rng>,
                typename O = uncvref_t<ORef>,
                CONCEPT_REQUIRES_(Range<Rng>() && PartialSummable<I, O, BOp, P>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng && rng, ORef && result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), static_cast<ORef&&>(result),
                               std::move(bop), std::move(proj));
            }

            template<typename Rng, typename ORng, typename BOp = plus,
                typename P = ident, typename I = iterator_t<Rng>,
                typename O = iterator_t<ORng>,
                CONCEPT_REQUIRES_(Range<Rng>() && Range<ORng>() &&
                    PartialSummable<I, O, BOp, P>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>),
                tag::out(safe_iterator_t<ORng>)>
            operator()(Rng && rng, ORng && result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), begin(result), end(result),
                               std::move(bop), std::move(proj));
            }
        };

        RANGES_INLINE_VARIABLE(partial_sum_fn, partial_sum)
    }
}

#endif
