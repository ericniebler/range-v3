/// \file
// Range v3 library
//
//  Copyright Rostislav Khlebnikov 2017
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_NUMERIC_inclusive_scan_HPP
#define RANGES_V3_NUMERIC_inclusive_scan_HPP

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
#include <range/v3/numeric/partial_sum.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail {
            template<typename AccT, typename BOp, typename I1, typename I2>
            using IndirectInvocableAndAssignable = meta::strict_and<
                IndirectRegularInvocable<BOp, I1, I2>,
                Assignable<detail::decay_t<AccT>&, indirect_result_of_t<BOp&(I1, I2)>>>;
        }
        
        template<typename I, typename O, typename BOp = plus, typename P = ident, typename T = void, 
            typename IProj = projected<projected<I, detail::as_value_type_t<I>>, P>,
            typename AccT = meta::if_c<std::is_void<T>::value, value_type_t<IProj>, T>>
        using InclusiveScannable = meta::strict_and<
            InputIterator<I>,
            OutputIterator<O, AccT const&>,
            MoveConstructible<AccT>,
            detail::IndirectInvocableAndAssignable<AccT, BOp, AccT*, AccT*>,
            detail::IndirectInvocableAndAssignable<AccT, BOp, AccT*, IProj>,
            detail::IndirectInvocableAndAssignable<AccT, BOp, IProj, IProj>>;


        struct inclusive_scan_fn
        {
            // No-init version
            template<typename I, typename S, typename O, typename S2,
                typename BOp = plus, typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S, I>() && Sentinel<S2, O>() &&
                    InclusiveScannable<I, O, BOp, P>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, S2 end_result, BOp bop = BOp{}, P proj = P{}) const
            {
                return detail::partial_sum_no_check(std::move(begin), std::move(end), 
                    std::move(result), std::move(end_result),
                    std::move(bop), std::move(proj));
            }

            template<typename I, typename S, typename O, typename BOp = plus,
                typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S, I>() && InclusiveScannable<I, O, BOp, P>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(std::move(begin), std::move(end), std::move(result), 
                               unreachable{}, std::move(bop), std::move(proj));
            }

             template<typename Rng, typename ORef, typename BOp = plus,
                typename P = ident, typename I = iterator_t<Rng>,
                typename O = uncvref_t<ORef>,
                CONCEPT_REQUIRES_(Range<Rng>() && InclusiveScannable<I, O, BOp, P>())>
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
                    InclusiveScannable<I, O, BOp, P>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>),
                tag::out(safe_iterator_t<ORng>)>
            operator()(Rng && rng, ORng && result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), begin(result), end(result),
                               std::move(bop), std::move(proj));
            }
            
            // init version
            template<typename I, typename S, typename O, typename S2,
                typename BOp, typename T, typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S, I>() && Sentinel<S2, O>() &&
                    InclusiveScannable<I, O, BOp, P, T>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, S2 end_result, BOp bop, T init, P proj = P{}) const
            {
                coerce<value_type_t<I>> v;

                if(begin != end && result != end_result)
                {
                    for(; begin != end && result != end_result;
                        ++begin, ++result)
                    {
                        auto &&cur = v(*begin);
                        init = invoke(bop, init, invoke(proj, cur));
                        *result = init;
                    }
                }
                return {begin, result};
            }

            template<typename I, typename S, typename O, typename BOp,
                typename T, typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S, I>() && InclusiveScannable<I, O, BOp, P, T>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, BOp bop, T init, P proj = P{}) const
            {
                return (*this)(std::move(begin), std::move(end), std::move(result), 
                               unreachable{}, std::move(bop), std::move(init), std::move(proj));
            }

             template<typename Rng, typename ORef, typename BOp, typename T,
                typename P = ident, typename I = iterator_t<Rng>,
                typename O = uncvref_t<ORef>,
                CONCEPT_REQUIRES_(Range<Rng>() && InclusiveScannable<I, O, BOp, P, T>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng && rng, ORef && result, BOp bop, T init, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), static_cast<ORef&&>(result),
                               std::move(bop), std::move(init), std::move(proj));
            }

            template<typename Rng, typename ORng, typename BOp, typename T,
                typename P = ident, typename I = iterator_t<Rng>,
                typename O = iterator_t<ORng>,
                CONCEPT_REQUIRES_(Range<Rng>() && Range<ORng>() &&
                    InclusiveScannable<I, O, BOp, P, T>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>),
                tag::out(safe_iterator_t<ORng>)>
            operator()(Rng && rng, ORng && result, BOp bop, T init, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), begin(result), end(result),
                               std::move(bop), std::move(init), std::move(proj));
            }
            
        };

        RANGES_INLINE_VARIABLE(inclusive_scan_fn, inclusive_scan)
    }
}

#endif
