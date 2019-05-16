/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP
#define RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/functional/comparisons.hpp>
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
    struct binary_search_fn
    {
        /// \brief function template \c binary_search_fn::operator()
        ///
        /// range-based version of the \c binary_search std algorithm
        ///
        /// \pre `Rng` is a model of the `Range` concept
        template<typename I, typename S, typename V, typename C = less, typename P = identity>
        auto operator()(I begin, S end, V const &val, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires ForwardIterator<I> && Sentinel<S, I> &&
                    IndirectStrictWeakOrder<C, V const *, projected<I, P>>)
        {
            begin = lower_bound(std::move(begin), end, val, std::ref(pred), std::ref(proj));
            return begin != end && !invoke(pred, val, invoke(proj, *begin));
        }

        /// \overload
        template<typename Rng, typename V, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, V const &val, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires ForwardRange<Rng> &&
                    IndirectStrictWeakOrder<C, V const *, projected<iterator_t<Rng>, P>>)
        {
            static_assert(!is_infinite<Rng>::value,
                "Trying to binary search an infinite range");
            return (*this)(begin(rng), end(rng), val, std::move(pred), std::move(proj));
        }
    };

    /// \sa `binary_search_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(binary_search_fn, binary_search)

    namespace cpp20
    {
        using ranges::binary_search;
    }
    /// @}
} // namespace ranges

#endif // include guard
