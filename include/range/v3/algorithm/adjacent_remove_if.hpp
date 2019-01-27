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
    struct adjacent_remove_if_fn
    {
        /// \brief function \c adjacent_remove_if_fn::operator()
        ///
        /// range-based version of the \c adjacent_remove_if algorithm
        ///
        /// \pre `Rng` is a model of the `ForwardRange` concept.
        /// \pre `Pred` is a model of the `BinaryPredicate` concept.
        template<typename I, typename S, typename Pred, typename Proj = identity>
        auto operator()(I first, S last, Pred pred = {}, Proj proj = {}) const ->
            CPP_ret(I)(
                requires Permutable<I> && Sentinel<S, I> &&
                    IndirectRelation<Pred, projected<I, Proj>>)
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
        auto operator()(Rng &&rng, Pred pred, Proj proj = {}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires ForwardRange<Rng> &&
                    IndirectRelation<Pred, projected<iterator_t<Rng>, Proj>> &&
                    Permutable<iterator_t<Rng>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `adjacent_remove_if_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(adjacent_remove_if_fn, adjacent_remove_if)
    /// @}
} // namespace ranges

#endif // RANGES_V3_ALGORITHM_ADJACENT_REMOVE_IF_HPP
