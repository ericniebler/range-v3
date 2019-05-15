/// \file
// Range v3 library
//
//  Copyright Johel Guerrero 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 0.0. (See accompanying
//  file LICENSE_0_0.txt or copy at
//  http://www.boost.org/LICENSE_0_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_ENDS_WITH_HPP
#define RANGES_V3_ALGORITHM_ENDS_WITH_HPP

#include <utility>
#include <concepts/concepts.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct ends_with_fn
    {
        template<typename I0, typename S0, typename I1, typename S1, typename C = equal_to,
            typename P0 = identity, typename P1 = identity>
        constexpr /*c++14*/ auto operator()(I0 begin0, S0 end0, I1 begin1, S1 end1,
                C pred = C{}, P0 proj0 = P0{}, P1 proj1 = P1{}) const ->
            CPP_ret(bool)(
                requires ((ForwardIterator<I0> && Sentinel<S0, I0>) ||
                     (InputIterator<I0> && SizedSentinel<S0, I0>)) &&
                    ((ForwardIterator<I1> && Sentinel<S1, I1>) ||
                     (InputIterator<I1> && SizedSentinel<S1, I1>)) &&
                    IndirectlyComparable<I0, I1, C, P0, P1>)
        {
            const auto drop = distance(begin0, end0) - distance(begin1, end1);
            if (drop < 0)
                return false;
            return equal(next(std::move(begin0), drop), std::move(end0), std::move(begin1),
                std::move(end1), std::move(pred), std::move(proj0), std::move(proj1));
        }

        template<typename Rng0, typename Rng1, typename C = equal_to, typename P0 = identity,
            typename P1 = identity>
        constexpr /*c++14*/ auto operator()(Rng0 &&rng0, Rng1 &&rng1, C pred = C{},
                P0 proj0 = P0{}, P1 proj1 = P1{}) const ->
            CPP_ret(bool)(
                requires (ForwardRange<Rng0> || (InputRange<Rng0> && SizedRange<Rng0>)) &&
                    (ForwardRange<Rng1> || (InputRange<Rng1> && SizedRange<Rng1>)) &&
                    IndirectlyComparable<iterator_t<Rng0>, iterator_t<Rng1>, C, P0, P1>)
        {
            const auto drop = distance(rng0) - distance(rng1);
            if (drop < 0)
                return false;
            return equal(next(begin(rng0), drop), end(rng0), begin(rng1), end(rng1),
                std::move(pred), std::move(proj0), std::move(proj1));
        }
    };

    /// \sa `ends_with_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(ends_with_fn, ends_with)
    /// @}
} // namespace ranges

#endif // include guard
