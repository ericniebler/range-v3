/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_LOWER_BOUND_HPP
#define RANGES_V3_ALGORITHM_LOWER_BOUND_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/aux_/lower_bound_n.hpp>
#include <range/v3/algorithm/partition_point.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct lower_bound_fn
    {
        template<typename I, typename S, typename V, typename C = less,
                 typename P = identity>
        auto operator()(I begin, S end, V const & val, C pred = C{}, P proj = P{}) const
            -> CPP_ret(I)( //
                requires forward_iterator<I> && sentinel_for<S, I> &&
                    indirect_strict_weak_order<C, V const *, projected<I, P>>)
        {
            return partition_point(std::move(begin),
                                   std::move(end),
                                   detail::make_lower_bound_predicate(pred, val),
                                   std::move(proj));
        }

        template<typename Rng, typename V, typename C = less, typename P = identity>
        auto operator()(Rng && rng, V const & val, C pred = C{}, P proj = P{}) const
            -> CPP_ret(safe_iterator_t<Rng>)( //
                requires forward_range<Rng> &&
                    indirect_strict_weak_order<C, V const *, projected<iterator_t<Rng>, P>>)
        {
            return partition_point(
                rng, detail::make_lower_bound_predicate(pred, val), std::move(proj));
        }
    };

    /// \sa `lower_bound_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(lower_bound_fn, lower_bound)

    namespace cpp20
    {
        using ranges::lower_bound;
    }
    /// @}
} // namespace ranges

#endif // include guard
