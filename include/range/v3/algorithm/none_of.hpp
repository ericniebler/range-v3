/// \file
// Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_NONE_OF_HPP
#define RANGES_V3_ALGORITHM_NONE_OF_HPP

#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(none_of)

        /// \brief function template \c none_of
        template(typename I, typename S, typename F, typename P = identity)(
            /// \pre
            requires input_iterator<I> AND sentinel_for<S, I> AND
            indirect_unary_predicate<F, projected<I, P>>)
        bool RANGES_FUNC(none_of)(I first, S last, F pred, P proj = P{}) //
        {
            for(; first != last; ++first)
                if(invoke(pred, invoke(proj, *first)))
                    return false;
            return true;
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(
            /// \pre
            requires input_range<Rng> AND
            indirect_unary_predicate<F, projected<iterator_t<Rng>, P>>)
        bool RANGES_FUNC(none_of)(Rng && rng, F pred, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_FUNC_END(none_of)

    namespace cpp20
    {
        using ranges::none_of;
    }
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
