///////////////////////////////////////////////////////////////////////////////
/// \file inner_product.hpp
///   Contains range-based versions of the std generic numeric operation:
///     iota.
//
// Copyright 2004 Eric Niebler.
// Copyright 2006 Thorsten Ottosen.
// Copyright 2009 Neil Groves.
// Copyright 2014 Gonzalo Brito Gadeschi.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_NUMERIC_INNER_PRODUCT_HPP
#define RANGES_V3_NUMERIC_INNER_PRODUCT_HPP

#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/range_algorithm.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
       template<typename I1, typename I2, typename T,
                typename BOp1 = plus, typename BOp2 = multiplies,
                typename P1 = ident, typename P2 = P1,
            typename V1 = iterator_value_t<I1>,
            typename V2 = iterator_value_t<I2>,
            typename X1 = concepts::Invokable::result_t<P1, V1>,
            typename X2 = concepts::Invokable::result_t<P2, V2>,
            typename Y2 = concepts::Invokable::result_t<BOp2, V1, V2>,
            typename Y1 = concepts::Invokable::result_t<BOp1, T, Y2>
                >
        constexpr bool Inner_Productable()
        {
            return InputIterator<I1>() &&
                InputIterator<I2>() &&
                Invokable<P1, V1>() &&
                Invokable<P2, V2>() &&
                Invokable<BOp2, V1, V2>() &&
                Invokable<BOp1, T, Y2>() &&
                Assignable<T &, Y2>();
        }

        struct inner_product_fn
        {
          template<typename I1, typename S1, typename I2, typename T,
                   typename BOp1 = plus, typename BOp2 = multiplies,
                   typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Sentinel<S1, I1>() &&
                                  Inner_Productable<I1, I2, T, BOp1, BOp2, P1, P2>())>
          T operator()(I1 begin1, S1 end1, I2 begin2, T init,
                       BOp1 bop1_ = BOp1{}, BOp2 bop2_ = BOp2{},
                       P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                auto &&bop1 = invokable(bop1_);
                auto &&bop2 = invokable(bop2_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);

                for (; begin1 != end1; ++begin1, ++begin2)
                  init = bop1(init, bop2(proj1(*begin1), proj2(*begin2)));
                return init;
            }

          template<typename Rng1, typename Rng2, typename T,
                   typename BOp1 = plus, typename BOp2 = multiplies,
                   typename P1 = ident, typename P2 = ident,
                   typename I1 = range_iterator_t<Rng1>,
                   typename I2 = range_iterator_t<Rng2>,
                   CONCEPT_REQUIRES_(Iterable<Rng1>() && Iterable<Rng2>() &&
                                     Inner_Productable<I1, I2, T, BOp1, BOp2, P1, P2>())>
          T operator()(Rng1 && rng1, Rng2 && rng2, T init,
                       BOp1 bop1 = BOp1{}, BOp2 bop2 = BOp2{},
                       P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
              return (*this)(begin(rng1), end(rng1), begin(rng2), std::move(init),
                             std::move(bop1), std::move(bop2), 
                             std::move(proj1), std::move(proj2));
            }
        };

        RANGES_CONSTEXPR range_algorithm<inner_product_fn> inner_product{};
    }
}

#endif
