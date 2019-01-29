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
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct partial_sort_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, I middle, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires Sortable<I, C, P> && RandomAccessIterator<I> && Sentinel<S, I>)
        {
            make_heap(begin, middle, std::ref(pred), std::ref(proj));
            auto const len = middle - begin;
            I i = middle;
            for(; i != end; ++i)
            {
                if(invoke(pred, invoke(proj, *i), invoke(proj, *begin)))
                {
                    iter_swap(i, begin);
                    detail::sift_down_n(begin, len, begin, std::ref(pred), std::ref(proj));
                }
            }
            sort_heap(begin, middle, std::ref(pred), std::ref(proj));
            return i;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, iterator_t<Rng> middle, C pred = C{},
                P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires Sortable<iterator_t<Rng>, C, P> && RandomAccessRange<Rng>)
        {
            return (*this)(begin(rng), std::move(middle), end(rng), std::move(pred),
                std::move(proj));
        }
    };

    /// \sa `partial_sort_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(partial_sort_fn, partial_sort)
    /// @}
} // namespace ranges

#endif // include guard
