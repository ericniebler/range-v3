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
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct equal_fn
    {
    private:
        template<typename I0, typename S0, typename I1, typename S1,
            typename C, typename P0, typename P1>
        constexpr /*c++14*/
        bool nocheck(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred,
            P0 proj0, P1 proj1) const
        {
            for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
                if(!invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                    return false;
            return begin0 == end0 && begin1 == end1;
        }

    public:
        template<typename I0, typename S0, typename I1, typename C = equal_to,
            typename P0 = identity, typename P1 = identity>
        RANGES_DEPRECATED("Use the variant of ranges::equal that takes an upper bound for "
            "both sequences")
        constexpr /*c++14*/ auto operator()(I0 begin0, S0 end0, I1 begin1, C pred = C{},
                P0 proj0 = P0{}, P1 proj1 = P1{}) const ->
            CPP_ret(bool)(
                requires InputIterator<I0> && Sentinel<S0, I0> && InputIterator<I1> &&
                    IndirectlyComparable<I0, I1, C, P0, P1>)
        {
            for(; begin0 != end0; ++begin0, ++begin1)
                if(!invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                    return false;
            return true;
        }

        template<typename I0, typename S0, typename I1, typename S1, typename C = equal_to,
            typename P0 = identity, typename P1 = identity>
        constexpr /*c++14*/ auto operator()(I0 begin0, S0 end0, I1 begin1, S1 end1,
                C pred = C{}, P0 proj0 = P0{}, P1 proj1 = P1{}) const ->
            CPP_ret(bool)(
                requires InputIterator<I0> && Sentinel<S0, I0> &&
                    InputIterator<I1> && Sentinel<S1, I1> &&
                    IndirectlyComparable<I0, I1, C, P0, P1>)
        {
            if RANGES_CONSTEXPR_IF(SizedSentinel<S0, I0> && SizedSentinel<S1, I1>)
                if(distance(begin0, end0) != distance(begin1, end1))
                    return false;
            return this->nocheck(std::move(begin0), std::move(end0), std::move(begin1),
                std::move(end1), std::move(pred), std::move(proj0), std::move(proj1));
        }

        template<typename Rng0, typename I1Ref, typename C = equal_to, typename P0 = identity,
            typename P1 = identity>
        RANGES_DEPRECATED("Use the variant of ranges::equal that takes an upper bound for "
            "both sequences")
        constexpr /*c++14*/ auto operator()(Rng0 &&rng0, I1Ref &&begin1, C pred = C{},
                P0 proj0 = P0{}, P1 proj1 = P1{}) const ->
            CPP_ret(bool)(
                requires InputRange<Rng0> && InputIterator<uncvref_t<I1Ref>> &&
                    IndirectlyComparable<iterator_t<Rng0>, uncvref_t<I1Ref>, C, P0, P1>)
        {
            RANGES_DIAGNOSTIC_PUSH
            RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
            return (*this)(begin(rng0), end(rng0), (I1Ref &&) begin1, std::move(pred),
                std::move(proj0), std::move(proj1));
            RANGES_DIAGNOSTIC_POP
        }

        template<typename Rng0, typename Rng1, typename C = equal_to, typename P0 = identity,
            typename P1 = identity>
        constexpr /*c++14*/ auto operator()(Rng0 &&rng0, Rng1 &&rng1, C pred = C{},
                P0 proj0 = P0{}, P1 proj1 = P1{}) const ->
            CPP_ret(bool)(
                requires InputRange<Rng0> && InputRange<Rng1> &&
                    IndirectlyComparable<iterator_t<Rng0>, iterator_t<Rng1>, C, P0, P1>)
        {
            if RANGES_CONSTEXPR_IF (SizedRange<Rng0> && SizedRange<Rng1>)
                if(distance(rng0) != distance(rng1))
                    return false;
            return this->nocheck(begin(rng0), end(rng0), begin(rng1), end(rng1),
                std::move(pred), std::move(proj0), std::move(proj1));
        }
    };

    /// \sa `equal_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(equal_fn, equal)
    /// @}
} // namespace ranges

#endif // include guard
