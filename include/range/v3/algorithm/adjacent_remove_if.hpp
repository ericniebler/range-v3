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

#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
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
        /// \pre `R` is a model of the `ForwardRange` concept.
        /// \pre `Pred` is a model of the `BinaryPredicate` concept.
        template<typename I, typename S, typename Pred, typename Proj = identity>
        auto operator()(I first, S last, Pred pred = {}, Proj proj = {}) const ->
            CPP_ret(I)(
                requires Permutable<I> && Sentinel<S, I> &&
                IndirectRelation<Pred, projected<I, Proj>>)
        {
            first = adjacent_find(std::move(first), last, std::ref(pred), std::ref(proj));
            if (first != last)
            {
                auto i = next(first);
                if (i != last)
                {
                    for (auto j = next(i); j != last; ++i, (void)++j)
                    {
                        if (!invoke(pred, invoke(proj, *i), invoke(proj, *j)))
                        {
                            *first = iter_move(i);
                            ++first;
                        }
                    }
                    *first++ = iter_move(i);
                }
            }
            return first;
        }

        /// \overload
        template<typename R, typename Pred, typename Proj = identity>
        auto operator()(R &&r, Pred pred, Proj proj = {}) const ->
            CPP_ret(safe_iterator_t<R>)(
                requires ForwardRange<R> && Permutable<iterator_t<R>> &&
                IndirectRelation<Pred, projected<iterator_t<R>, Proj>>)
        {
            return (*this)(begin(r), end(r), std::move(pred), std::move(proj));
        }
    };

    /// \sa `adjacent_remove_if_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(with_braced_init_args<adjacent_remove_if_fn>,
                                    adjacent_remove_if)
    /// @}
} // namespace ranges

#endif // RANGES_V3_ALGORITHM_ADJACENT_REMOVE_IF_HPP
