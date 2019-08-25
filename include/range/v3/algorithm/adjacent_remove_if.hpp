/// \file
// Range v3 library
//
//  Copyright Eric Niebler
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_ADJACENT_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_REMOVE_IF_HPP

#include <functional>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(adjacent_remove_if)
        /// \brief function \c adjacent_remove_if
        ///
        /// range-based version of the \c adjacent_remove_if algorithm
        ///
        /// \pre `Rng` is a model of the `forward_range` concept.
        /// \pre `Pred` is a model of the `BinaryPredicate` concept.
        template<typename I, typename S, typename Pred, typename Proj = identity>
        auto RANGES_FUN_NIEBLOID(adjacent_remove_if)(
            I first, S last, Pred pred = {}, Proj proj = {})
            ->CPP_ret(I)( //
                requires permutable<I> && sentinel_for<S, I> &&
                indirect_relation<Pred, projected<I, Proj>>)
        {
            first = adjacent_find(std::move(first), last, std::ref(pred), std::ref(proj));
            if(first == last)
                return first;

            auto i = first;
            for(auto j = ++i; ++j != last; ++i)
            {
                if(!invoke(pred, invoke(proj, *i), invoke(proj, *j)))
                {
                    *first = iter_move(i);
                    ++first;
                }
            }

            *first = iter_move(i);
            ++first;
            return first;
        }

        /// \overload
        template<typename Rng, typename Pred, typename Proj = identity>
        auto RANGES_FUN_NIEBLOID(adjacent_remove_if)(
            Rng && rng, Pred pred, Proj proj = {}) //
            ->CPP_ret(safe_iterator_t<Rng>)(       //
                requires forward_range<Rng> &&
                indirect_relation<Pred, projected<iterator_t<Rng>, Proj>> &&
                permutable<iterator_t<Rng>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    RANGES_END_NIEBLOID(adjacent_remove_if)

    namespace cpp20
    {
        using ranges::adjacent_remove_if;
    }
    /// @}
} // namespace ranges

#endif // RANGES_V3_ALGORITHM_ADJACENT_REMOVE_IF_HPP
