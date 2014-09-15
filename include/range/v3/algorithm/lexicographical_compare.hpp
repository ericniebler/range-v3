//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct lexicographical_compare_fn
        {
            template<typename I0, typename S0, typename I1, typename S1,
                typename C = ordered_less, typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(IteratorRange<I0, S0>() && IteratorRange<I1, S1>() &&
                    Comparable<I0, I1, C, P0, P1>())>
            bool operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred_ = C{}, P0 proj0_ = P0{},
                P1 proj1_ = P1{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj0 = invokable(proj0_);
                auto &&proj1 = invokable(proj1_);
                for(; begin1 != end1; ++begin0, ++begin1)
                {
                    if(begin0 == end0 || pred(proj0(*begin0), proj1(*begin1)))
                        return true;
                    if(pred(proj1(*begin1), proj0(*begin0)))
                        return false;
                }
                return false;
            }

            template<typename Rng0, typename Rng1, typename C = ordered_less,
                typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = range_iterator_t<Rng1>,
                CONCEPT_REQUIRES_(InputIterable<Rng0>() && InputIterable<Rng1>() &&
                    Comparable<I0, I1, C, P0, P1>())>
            bool operator()(Rng0 &&rng0, Rng1 &&rng1, C pred = C{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), begin(rng1), end(rng1), std::move(pred),
                    std::move(proj0), std::move(proj1));
            }
        };

        RANGES_CONSTEXPR range_algorithm<lexicographical_compare_fn> lexicographical_compare{};

    } // namespace v3
} // namespace ranges

#endif // include guard
