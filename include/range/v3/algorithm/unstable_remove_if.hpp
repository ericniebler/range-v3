/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//  Copyright Casey Carter 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP

#include <functional>
#include <utility>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/reverse_iterator.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// unstable_remove have O(1) complexity for each element remove, unlike remove O(n)
    /// [for worst case]. Each erased element overwritten (moved in) with last one.
    /// unstable_remove_if does not preserve relative element order.
    RANGES_BEGIN_NIEBLOID(unstable_remove_if)

        /// \brief function template \c unstable_remove_if
        template<typename I, typename C, typename P = identity>
        auto RANGES_FUN_NIEBLOID(unstable_remove_if)(
            I first, I last, C pred, P proj = {}) //
            ->CPP_ret(I)(                         //
                requires bidirectional_iterator<I> && permutable<I> &&
                indirect_unary_predicate<C, projected<I, P>>)
        {
            while(true)
            {
                first = find_if(std::move(first), last, std::ref(pred), std::ref(proj));
                last = find_if_not(make_reverse_iterator(std::move(last)),
                                   make_reverse_iterator(first),
                                   std::ref(pred),
                                   std::ref(proj))
                           .base();
                if(first == last)
                    return first;
                *first = iter_move(--last);

                // discussion here: https://github.com/ericniebler/range-v3/issues/988
                ++first;
            }
        }

        /// \overload
        template<typename Rng, typename C, typename P = identity>
        auto RANGES_FUN_NIEBLOID(unstable_remove_if)(Rng && rng, C pred, P proj = P{}) //
            ->CPP_ret(safe_iterator_t<Rng>)(                                           //
                requires bidirectional_range<Rng> && common_range<Rng> &&
                permutable<iterator_t<Rng>> &&
                indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(unstable_remove_if)
    /// @}
} // namespace ranges

#endif // RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP
