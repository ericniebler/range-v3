/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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
#include <range/v3/utility/static_const.hpp>

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
            RANGES_CXX14_CONSTEXPR
            bool nocheck(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred,
                P0 proj0, P1 proj1) const
            {
                for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
                    if(!invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                        return false;
                return begin0 == end0 && begin1 == end1;
            }

        public:
            template<typename I0, typename S0, typename I1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S0, I0>() &&
                    Comparable<I0, I1, C, P0, P1>()
                )>
            RANGES_CXX14_CONSTEXPR
            bool operator()(I0 begin0, S0 end0, I1 begin1, C pred = C{},
                P0 proj0 = P0{}, P1 proj1 = P1{}) const
            {
                for(; begin0 != end0; ++begin0, ++begin1)
                    if(!invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                        return false;
                return true;
            }

            template<typename I0, typename S0, typename I1, typename S1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S0, I0>() && Sentinel<S1, I1>() &&
                    Comparable<I0, I1, C, P0, P1>()
                )>
            RANGES_CXX14_CONSTEXPR
            bool operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred = C{},
                P0 proj0 = P0{}, P1 proj1 = P1{}) const
            {
                if RANGES_CONSTEXPR_IF(SizedSentinel<S0, I0>() && SizedSentinel<S1, I1>())
                    if(distance(begin0, end0) != distance(begin1, end1))
                        return false;
                return this->nocheck(std::move(begin0), std::move(end0), std::move(begin1),
                    std::move(end1), std::move(pred), std::move(proj0), std::move(proj1));
            }

            template<typename Rng0, typename I1Ref,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                typename I0 = iterator_t<Rng0>,
                typename I1 = uncvref_t<I1Ref>,
                CONCEPT_REQUIRES_(
                    Range<Rng0>() && Iterator<I1>() &&
                    Comparable<I0, I1, C, P0, P1>()
                )>
            RANGES_CXX14_CONSTEXPR
            bool operator()(Rng0 && rng0, I1Ref && begin1, C pred = C{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), (I1Ref &&) begin1, std::move(pred),
                    std::move(proj0), std::move(proj1));
            }

            template<typename Rng0, typename Rng1,
                typename C = equal_to, typename P0 = ident, typename P1 = ident,
                typename I0 = iterator_t<Rng0>,
                typename I1 = iterator_t<Rng1>,
                CONCEPT_REQUIRES_(
                    Range<Rng0>() && Range<Rng1>() &&
                    Comparable<I0, I1, C, P0, P1>()
                )>
            RANGES_CXX14_CONSTEXPR
            bool operator()(Rng0 && rng0, Rng1 && rng1, C pred = C{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                if RANGES_CONSTEXPR_IF (SizedRange<Rng0>() && SizedRange<Rng1>())
                    if(distance(rng0) != distance(rng1))
                        return false;
                return this->nocheck(begin(rng0), end(rng0), begin(rng1), end(rng1),
                    std::move(pred), std::move(proj0), std::move(proj1));
            }
        };

        /// \sa `equal_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<equal_fn>, equal)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
