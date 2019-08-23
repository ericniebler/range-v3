/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
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
    struct adjacent_find_fn
    {
        /// \brief function template \c adjacent_find_fn::operator()
        ///
        /// range-based version of the \c adjacent_find std algorithm
        ///
        /// \pre `Rng` is a model of the `Range` concept
        /// \pre `C` is a model of the `BinaryPredicate` concept
        template<typename I, typename S, typename C = equal_to, typename P = identity>
        auto operator()(I first, S last, C pred = C{}, P proj = P{}) const
            -> CPP_ret(I)( //
                requires forward_iterator<I> && sentinel_for<S, I> &&
                    indirect_relation<C, projected<I, P>>)
        {
            if(first == last)
                return first;
            auto inext = first;
            for(; ++inext != last; first = inext)
                if(invoke(pred, invoke(proj, *first), invoke(proj, *inext)))
                    return first;
            return inext;
        }

        /// \overload
        template<typename Rng, typename C = equal_to, typename P = identity>
        auto operator()(Rng && rng, C pred = C{}, P proj = P{}) const
            -> CPP_ret(safe_iterator_t<Rng>)( //
                requires forward_range<Rng> &&
                    indirect_relation<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `adjacent_find_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(adjacent_find_fn, adjacent_find)

    namespace cpp20
    {
        using ranges::adjacent_find;
    }
    /// @}
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
