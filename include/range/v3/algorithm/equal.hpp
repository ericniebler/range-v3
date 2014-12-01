// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 0.0. (See accompanying
//  file LICENSE_0_0.txt or copy at
//  http://www.boost.org/LICENSE_0_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_EQUAL_HPP
#define RANGES_V3_ALGORITHM_EQUAL_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
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
                    IteratorRange<I0, S0>() &&
                    WeaklyComparable<I0, I1, C, P0, P1>()
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
                    IteratorRange<I0, S0>() && IteratorRange<I1, S1>() &&
                    Comparable<I0, I1, C, P0, P1>()
                )>
            bool operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred_ = C{},
                P0 proj0_ = P0{}, P1 proj1_ = P1{}) const
            {
                if(SizedIteratorRange<I0, S0>() && SizedIteratorRange<I1, S1>())
                    if(distance(begin0, end0) != distance(begin1, end1))
                        return false;
                return this->nocheck(std::move(begin0), std::move(end0), std::move(begin1),
                    std::move(end1), std::move(pred_), std::move(proj0_), std::move(proj1_));
            }

            template<typename Rng0, typename I1Ref,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = uncvref_t<I1Ref>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng0>() &&
                    WeaklyComparable<I0, I1, C, P0, P1>()
                )>
            bool operator()(Rng0 && rng0, I1Ref && begin1, C pred_ = C{}, P0 proj0_ = P0{},
                P1 proj1_ = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), (I1Ref &&) begin1, std::move(pred_),
                    std::move(proj0_), std::move(proj1_));
            }

            template<typename Rng0, typename Rng1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = range_iterator_t<Rng1>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng0>() && Iterable<Rng1>() &&
                    Comparable<I0, I1, C, P0, P1>()
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

        /// \sa `equal_fn`
        /// \ingroup group-algorithms
        constexpr with_braced_init_args<equal_fn> equal{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
