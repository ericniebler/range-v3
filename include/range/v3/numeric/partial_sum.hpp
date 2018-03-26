/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
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
        /// \cond
        namespace detail
        {
            // Only needed for type-checking purposes:
            struct as_lvalue_fn
            {
                template<typename T>
                constexpr T &operator()(T &&t) const noexcept
                {
                    return t;
                }
            };
            template<typename I>
            using as_value_type_t = composed<as_lvalue_fn, coerce<value_type_t<I>>>;
        }
        /// \endcond

        // axiom: BOp is associative over values of I.
        template<typename I, typename BOp>
        using IndirectSemigroup = meta::strict_and<
            Readable<I>,
            Copyable<value_type_t<I>>,
            IndirectRegularInvocable<composed<coerce<value_type_t<I>>, BOp>, value_type_t<I>*, I>>;

        template<typename I, typename O, typename BOp = plus, typename P = ident,
            typename X = projected<projected<I, detail::as_value_type_t<I>>, P>>
        using PartialSummable = meta::strict_and<
            InputIterator<I>,
            IndirectSemigroup<X, BOp>,
            OutputIterator<O, value_type_t<X> const &>>;

        struct partial_sum_fn
        {
            template<typename I, typename S1, typename O, typename S2,
                typename BOp = plus, typename P = ident,
                CONCEPT_REQUIRES_(Sentinel<S1, I>() && Sentinel<S2, O>() &&
                    PartialSummable<I, O, BOp, P>())>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S1 end, O result, S2 end_result, BOp bop = BOp{}, P proj = P{}) const
            {
                using X = projected<projected<I, detail::as_value_type_t<I>>, P>;
                coerce<value_type_t<I>> val_i;
                coerce<value_type_t<X>> val_x;
                if(begin != end && result != end_result)
                {
                    auto &&cur1 = val_i(*begin);
                    value_type_t<X> t(invoke(proj, cur1));
                    *result = t;
                    for(++begin, ++result; begin != end && result != end_result;
                        ++begin, ++result)
                    {
                        auto &&cur2 = val_i(*begin);
                        t = val_x(invoke(bop, t, invoke(proj, cur2)));
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
