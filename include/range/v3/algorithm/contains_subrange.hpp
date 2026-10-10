/// \file
// Range v3 library
//
//  Copyright Google LLC, 2021
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_CONTAINS_SUBRANGE_HPP
#define RANGES_V3_ALGORITHM_CONTAINS_SUBRANGE_HPP

#include <utility>

#include <concepts/concepts.hpp>

#include <range/v3/algorithm/search.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// `contains_subrange` is a search-based algorithm that checks whether or
    /// not a given "needle" range is a subrange of a "haystack" range.
    ///
    /// Example usage:
    /// ```cpp
    /// auto const haystack = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    /// auto const needle = std::vector{3, 4, 5};
    /// assert(ranges::contains(haystack, needle));
    /// assert(ranges::contains(haystack, needle, ranges::less));
    ///
    /// auto const not_a_needle = std::vector{4, 5, 3};
    /// assert(not ranges::contains(haystack, not_a_needle));
    /// ```
    ///
    /// The interface supports both iterator-sentinel pairs and range objects.
    /// Due to multi-pass iteration, this algorithm requires both ranges to be
    /// forward ranges, and the elements' projections need to be comparable when
    /// using the predicate.
    RANGES_FUNC_BEGIN(contains_subrange)

        /// \brief function template \c contains
        template(typename I1, typename S1, typename I2, typename S2,
                 typename Pred = equal_to,
                 typename Proj1 = identity, typename Proj2 = identity)(
            /// \pre
            requires forward_iterator<I1> AND sentinel_for<S1, I1> AND
            forward_iterator<I2> AND sentinel_for<S2, I2> AND
            indirectly_comparable<I1, I2, Pred, Proj1, Proj2>)
        constexpr bool RANGES_FUNC(contains)(I1 first1, S1 last1,
                                             I2 first2, S2 last2,
                                             Pred pred = {},
                                             Proj1 proj1 = {},
                                             Proj2 proj2 = {})
        {
            return first2 == last2 ||
                   ranges::search(first1, last1, first2, last2, pred, proj1, proj2).empty() == false;
        }

        /// \overload
        template(typename Rng1, typename Rng2, typename Pred = equal_to,
                 typename Proj1 = identity, typename Proj2 = identity)(
            /// \pre
            requires forward_range<Rng1> AND forward_range<Rng2> AND
            indirectly_comparable<iterator_t<Rng1>, iterator_t<Rng2>, Pred, Proj1, Proj2>)
        constexpr bool RANGES_FUNC(contains)(Rng1 && rng1, Rng2 && rng2,
                                             Pred pred = {},
                                             Proj1 proj1 = {},
                                             Proj2 proj2 = {})
        {
            return (*this)(ranges::begin(rng1), ranges::end(rng1),
                           ranges::begin(rng2), ranges::end(rng2),
                           std::move(pred), std::move(proj1), std::move(proj2));
        }

    RANGES_FUNC_END(contains_subrange)
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif // RANGES_V3_ALGORITHM_CONTAINS_SUBRANGE_HPP
