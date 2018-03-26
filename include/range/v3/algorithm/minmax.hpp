/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_MINMAX_HPP
#define RANGES_V3_ALGORITHM_MINMAX_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct minmax_fn
        {
            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>, typename V = value_type_t<I>,
                typename R = tagged_pair<tag::min(V), tag::max(V)>,
                CONCEPT_REQUIRES_(InputRange<Rng>() && Copyable<V>() &&
                    IndirectRelation<C, projected<I, P>>())>
            RANGES_CXX14_CONSTEXPR
            R operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                auto begin = ranges::begin(rng);
                auto end = ranges::end(rng);
                RANGES_EXPECT(begin != end);
                auto result = R{*begin, *begin};
                if(++begin != end)
                {
                    {
                        auto && tmp = *begin;
                        if(invoke(pred, invoke(proj, tmp), invoke(proj, result.first)))
                            result.first = (decltype(tmp) &&) tmp;
                        else
                            result.second = (decltype(tmp) &&) tmp;
                    }
                    while(++begin != end)
                    {
                        V tmp1 = *begin;
                        if(++begin == end)
                        {
                            if(invoke(pred, invoke(proj, tmp1), invoke(proj, result.first)))
                                result.first = std::move(tmp1);
                            else if(!invoke(pred, invoke(proj, tmp1), invoke(proj, result.second)))
                                result.second = std::move(tmp1);
                            break;
                        }

                        auto && tmp2 = *begin;
                        if(invoke(pred, invoke(proj, tmp2), invoke(proj, tmp1)))
                        {
                            if(invoke(pred, invoke(proj, tmp2), invoke(proj, result.first)))
                                result.first = (decltype(tmp2) &&) tmp2;
                            if(!invoke(pred, invoke(proj, tmp1), invoke(proj, result.second)))
                                result.second = std::move(tmp1);
                        }
                        else
                        {
                            if(invoke(pred, invoke(proj, tmp1), invoke(proj, result.first)))
                                result.first = std::move(tmp1);
                            if(!invoke(pred, invoke(proj, tmp2), invoke(proj, result.second)))
                                result.second = (decltype(tmp2) &&) tmp2;
                        }
                    }
                }
                return result;
            }

            template<typename T, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    IndirectRelation<C, projected<const T *, P>>())>
            constexpr tagged_pair<tag::min(T const &), tag::max(T const &)>
            operator()(T const &a, T const &b, C pred = C{}, P proj = P{}) const
            {
                using R = tagged_pair<tag::min(T const &), tag::max(T const &)>;
                return invoke(pred, invoke(proj, b), invoke(proj, a)) ? R{b, a} : R{a, b};
            }
        };

        /// \sa `minmax_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<minmax_fn>, minmax)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
