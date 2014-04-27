//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 0.0. (See accompanying
//  file LICENSE_0_0.txt or copy at
//  http://www.boost.org/LICENSE_0_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_EQUAL_HPP
#define RANGES_V3_ALGORITHM_EQUAL_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I0, typename I1,
            typename C = equal_to, typename P0 = ident, typename P1 = ident,
            typename V0 = iterator_value_t<I0>,
            typename V1 = iterator_value_t<I1>,
            typename X0 = concepts::Invokable::result_t<P0, V0>,
            typename X1 = concepts::Invokable::result_t<P1, V1>>
        constexpr bool WeakRangeEqualityComparable()
        {
            return InputIterator<I0>() &&
                   WeakInputIterator<I1>() &&
                   Invokable<P0, V0>() &&
                   Invokable<P1, V1>() &&
                   InvokableRelation<C, X0, X1>();
        }

        template<typename I0, typename I1,
            typename C = equal_to, typename P0 = ident, typename P1 = ident>
        constexpr bool RangeEqualityComparable()
        {
            return InputIterator<I1>() &&
                WeakRangeEqualityComparable<I0, I1, C, P0, P1>();
        }

        struct equal_fn
        {
        private:
            template<typename I0, typename S0, typename I1, typename S1,
                typename C, typename P0, typename P1>
            bool nocheck(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred_,
                P0 proj0_, P1 proj1_) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj0 = invokable(proj0_);
                auto &&proj1 = invokable(proj1_);
                for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
                    if(!pred(proj0(*begin0), proj1(*begin1)))
                        return false;
                return begin0 == end0 && begin1 == end1;
            }

        public:
            template<typename I0, typename S0, typename I1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S0, I0>() &&
                    WeakRangeEqualityComparable<I0, I1, C, P0, P1>()
                )>
            bool operator()(I0 begin0, S0 end0, I1 begin1, C pred_ = C{},
                P0 proj0_ = P0{}, P1 proj1_ = P1{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj0 = invokable(proj0_);
                auto &&proj1 = invokable(proj1_);
                for(; begin0 != end0; ++begin0, ++begin1)
                    if(!pred(proj0(*begin0), proj1(*begin1)))
                        return false;
                return true;
            }

            template<typename I0, typename S0, typename I1, typename S1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S0, I0>() && Sentinel<S1, I1>() &&
                    RangeEqualityComparable<I0, I1, C, P0, P1>()
                )>
            bool operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred_ = C{},
                P0 proj0_ = P0{}, P1 proj1_ = P1{}) const
            {
                using R0 = iterator_range<I0, S0>;
                using R1 = iterator_range<I1, S1>;
                if(SizedIterable<R0>() && SizedIterable<R1>())
                    if(distance(begin0, end0) != distance(begin1, end1))
                        return false;
                return this->nocheck(std::move(begin0), std::move(end0), std::move(begin1),
                    std::move(end1), std::move(pred_), std::move(proj0_), std::move(proj1_));
            }

            template<typename Rng0, typename I1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng0>() &&
                    WeakRangeEqualityComparable<I0, I1, C, P0, P1>()
                )>
            bool operator()(Rng0 && rng0, I1 begin1, C pred_ = C{}, P0 proj0_ = P0{},
                P1 proj1_ = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), std::move(begin1), std::move(pred_),
                    std::move(proj0_), std::move(proj1_));
            }

            template<typename Rng0, typename Rng1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = range_iterator_t<Rng1>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng0>() && Iterable<Rng1>() &&
                    RangeEqualityComparable<I0, I1, C, P0, P1>()
                )>
            bool operator()(Rng0 && rng0, Rng1 && rng1, C pred_ = C{}, P0 proj0_ = P0{},
                P1 proj1_ = P1{}) const
            {
                if(SizedIterable<Rng0>() && SizedIterable<Rng1>())
                    if(distance(rng0) != distance(rng1))
                        return false;
                return this->nocheck(begin(rng0), end(rng0), begin(rng1), end(rng1),
                    std::move(pred_), std::move(proj0_), std::move(proj1_));
            }
        };

        RANGES_CONSTEXPR range_algorithm<equal_fn> equal{};

    } // namespace v3
} // namespace ranges

#endif // include guard
