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

#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
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
    RANGES_BEGIN_NIEBLOID(partial_sort)

        /// \brief function template \c partial_sort
        template<typename I, typename S, typename C = less, typename P = identity>
        auto RANGES_FUN_NIEBLOID(partial_sort)(
            I first, I middle, S last, C pred = C{}, P proj = P{}) //
            ->CPP_ret(I)(                                          //
                requires sortable<I, C, P> && random_access_iterator<I> &&
                sentinel_for<S, I>)
        {
            make_heap(first, middle, std::ref(pred), std::ref(proj));
            auto const len = middle - first;
            I i = middle;
            for(; i != last; ++i)
            {
                if(invoke(pred, invoke(proj, *i), invoke(proj, *first)))
                {
                    iter_swap(i, first);
                    detail::sift_down_n(
                        first, len, first, std::ref(pred), std::ref(proj));
                }
            }
            sort_heap(first, middle, std::ref(pred), std::ref(proj));
            return i;
        }

        /// \overload
        template<typename Rng, typename C = less, typename P = identity>
        auto RANGES_FUN_NIEBLOID(partial_sort)(
            Rng && rng, iterator_t<Rng> middle, C pred = C{}, P proj = P{}) //
            ->CPP_ret(safe_iterator_t<Rng>)(                                //
                requires sortable<iterator_t<Rng>, C, P> && random_access_range<Rng>)
        {
            return (*this)(begin(rng),
                           std::move(middle),
                           end(rng),
                           std::move(pred),
                           std::move(proj));
        }

    RANGES_END_NIEBLOID(partial_sort)

    namespace cpp20
    {
        using ranges::partial_sort;
    }
    /// @}
} // namespace ranges

#endif // include guard
