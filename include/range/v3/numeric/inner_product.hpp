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
#ifndef RANGES_V3_NUMERIC_INNER_PRODUCT_HPP
#define RANGES_V3_NUMERIC_INNER_PRODUCT_HPP

#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
       template<typename I1, typename I2, typename T,
                typename BOp1 = plus, typename BOp2 = multiplies,
                typename P1 = ident, typename P2 = ident,
                typename V1 = value_type_t<I1>,
                typename V2 = value_type_t<I2>,
                typename X1 = concepts::Invocable::result_t<P1&, V1>,
                typename X2 = concepts::Invocable::result_t<P2&, V2>,
                typename Y2 = concepts::Invocable::result_t<BOp2&, X1, X2>,
                typename Y1 = concepts::Invocable::result_t<BOp1&, T, Y2>>
        using InnerProductable = meta::strict_and<
            InputIterator<I1>,
            InputIterator<I2>,
            Invocable<P1&, V1>,
            Invocable<P2&, V2>,
            Invocable<BOp2&, X1, X2>,
            Invocable<BOp1&, T, Y2>,
            Assignable<T&, Y2>>;

        struct inner_product_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename T,
                typename BOp1 = plus, typename BOp2 = multiplies,
                typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S1, I1>() &&
                    Sentinel<S2, I2>() &&
                    InnerProductable<I1, I2, T, BOp1, BOp2, P1, P2>()
                )>
            T operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, T init, BOp1 bop1 = BOp1{},
                BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                  init = invoke(bop1, init, invoke(bop2, invoke(proj1, *begin1), invoke(proj2, *begin2)));
                return init;
            }

            template<typename I1, typename S1, typename I2, typename T,
                typename BOp1 = plus, typename BOp2 = multiplies,
                typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S1, I1>() &&
                    InnerProductable<I1, I2, T, BOp1, BOp2, P1, P2>()
                )>
            T operator()(I1 begin1, S1 end1, I2 begin2, T init, BOp1 bop1 = BOp1{},
                BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(std::move(begin1), std::move(end1), std::move(begin2), unreachable{},
                    std::move(init), std::move(bop1), std::move(bop2), std::move(proj1), std::move(proj2));
            }

            template<typename Rng1, typename I2Ref, typename T, typename BOp1 = plus,
                typename BOp2 = multiplies, typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = uncvref_t<I2Ref>,
                CONCEPT_REQUIRES_(
                    Range<Rng1>() && Iterator<I2>() &&
                    InnerProductable<I1, I2, T, BOp1, BOp2, P1, P2>()
                )>
            T operator()(Rng1 && rng1, I2Ref && begin2, T init, BOp1 bop1 = BOp1{},
                BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), static_cast<I2Ref&&>(begin2), std::move(init),
                    std::move(bop1), std::move(bop2),  std::move(proj1), std::move(proj2));
            }

            template<typename Rng1, typename Rng2, typename T, typename BOp1 = plus,
                typename BOp2 = multiplies, typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(
                    Range<Rng1>() &&
                    Range<Rng2>() &&
                    InnerProductable<I1, I2, T, BOp1, BOp2, P1, P2>()
                )>
            T operator()(Rng1 && rng1, Rng2 && rng2, T init, BOp1 bop1 = BOp1{},
                BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(init),
                    std::move(bop1), std::move(bop2),  std::move(proj1), std::move(proj2));
            }
        };

        RANGES_INLINE_VARIABLE(with_braced_init_args<inner_product_fn>,
                               inner_product)
    }
}

#endif
