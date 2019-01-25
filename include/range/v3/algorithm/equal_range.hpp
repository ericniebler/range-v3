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
#ifndef RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP
#define RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/upper_bound.hpp>
#include <range/v3/algorithm/aux_/equal_range_n.hpp>
#include <range/v3/algorithm/aux_/lower_bound_n.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct equal_range_fn
    {
        template<typename I, typename S, typename V, typename C = less,
            typename P = identity>
        auto operator()(I begin, S end, V const &val, C pred = C{}, P proj = P{}) const ->
            CPP_ret(subrange<I>)(
                requires ForwardIterator<I> && Sentinel<S, I> &&
                    IndirectStrictWeakOrder<C, V const *, projected<I, P>>)
        {
            if RANGES_CONSTEXPR_IF (SizedSentinel<S, I>)
            {
                auto const len = distance(begin, end);
                return aux::equal_range_n(std::move(begin), len, val,
                    std::move(pred), std::move(proj));
            }

            // Probe exponentially for either end-of-range, an iterator that
            // is past the equal range (i.e., denotes an element greater
            // than val), or is in the equal range (denotes an element equal
            // to val).
            auto dist = iter_difference_t<I>{1};
            while(true)
            {
                auto mid = begin;
                auto d = advance(mid, dist, end);
                if(d || mid == end)
                {
                    // at the end of the input range
                    dist -= d;
                    return aux::equal_range_n(
                        std::move(begin), dist, val, std::ref(pred), std::ref(proj));
                }
                // if val < *mid, mid is after the target range.
                auto && v = *mid;
                auto && pv = invoke(proj, (decltype(v)&&) v);
                if(invoke(pred, val, pv))
                {
                    return aux::equal_range_n(
                        std::move(begin), dist, val, std::ref(pred), std::ref(proj));
                }
                else if(!invoke(pred, pv, val))
                {
                    // *mid == val: the lower bound is <= mid, and the upper bound is > mid.
                    return {
                        aux::lower_bound_n(std::move(begin), dist, val,
                            std::ref(pred), std::ref(proj)),
                        upper_bound(std::move(mid), std::move(end), val,
                            std::ref(pred), std::ref(proj))
                    };
                }
                // *mid < val, mid is before the target range.
                begin = std::move(mid);
                ++begin;
                dist *= 2;
            }
        }

        template<typename Rng, typename V, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, V const &val, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_subrange_t<Rng>)(
                requires ForwardRange<Rng> &&
                    IndirectStrictWeakOrder<C, V const *, projected<iterator_t<Rng>, P>>)
        {
            if RANGES_CONSTEXPR_IF (SizedRange<Rng>)
            {
                auto const len = distance(rng);
                return aux::equal_range_n(begin(rng), len, val, std::move(pred), std::move(proj));
            }

            return (*this)(begin(rng), end(rng), val, std::move(pred), std::move(proj));
        }
    };

    /// \sa `equal_range_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(equal_range_fn, equal_range)
    /// @}
} // namespace ranges

#endif // include guard
