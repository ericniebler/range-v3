/// \file
// Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_ANY_OF_HPP
#define RANGES_V3_ALGORITHM_ANY_OF_HPP

#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct any_of_fn
    {
        template<typename I, typename S, typename F, typename P = identity>
        auto operator()(I first, S last, F pred, P proj = P{}) const -> CPP_ret(bool)( //
            requires input_iterator<I> && sentinel_for<S, I> &&
                indirect_unary_predicate<F, projected<I, P>>)
        {
            for(; first != last; ++first)
                if(invoke(pred, invoke(proj, *first)))
                    return true;
            return false;
        }

        template<typename Rng, typename F, typename P = identity>
        auto operator()(Rng && rng, F pred, P proj = P{}) const -> CPP_ret(bool)( //
            requires input_range<Rng> &&
                indirect_unary_predicate<F, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `any_of_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(any_of_fn, any_of)

    namespace cpp20
    {
        using ranges::any_of;
    }
    /// @}
} // namespace ranges

#endif // include guard
