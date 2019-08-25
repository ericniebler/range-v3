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
#ifndef RANGES_V3_ALGORITHM_COUNT_IF_HPP
#define RANGES_V3_ALGORITHM_COUNT_IF_HPP

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

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(count_if)

        /// \brief function template \c count_if
        template<typename I, typename S, typename R, typename P = identity>
        auto RANGES_FUN_NIEBLOID(count_if)(I first, S last, R pred, P proj = P{})
            ->CPP_ret(iter_difference_t<I>)( //
                requires input_iterator<I> && sentinel_for<S, I> &&
                indirect_unary_predicate<R, projected<I, P>>)
        {
            iter_difference_t<I> n = 0;
            for(; first != last; ++first)
                if(invoke(pred, invoke(proj, *first)))
                    ++n;
            return n;
        }

        /// \overload
        template<typename Rng, typename R, typename P = identity>
        auto RANGES_FUN_NIEBLOID(count_if)(Rng && rng, R pred, P proj = P{})
            ->CPP_ret(iter_difference_t<iterator_t<Rng>>)( //
                requires input_range<Rng> &&
                indirect_unary_predicate<R, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(count_if)

    namespace cpp20
    {
        using ranges::count_if;
    }
    /// @}
} // namespace ranges

#endif // include guard
