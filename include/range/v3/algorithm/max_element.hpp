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
#ifndef RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
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
    RANGES_BEGIN_NIEBLOID(max_element)

        /// \brief function template \c max_element
        template<typename I, typename S, typename C = less, typename P = identity>
        auto RANGES_FUN_NIEBLOID(max_element)(I first, S last, C pred = C{}, P proj = P{})
            ->CPP_ret(I)( //
                requires forward_iterator<I> && sentinel_for<S, I> &&
                indirect_strict_weak_order<C, projected<I, P>>)
        {
            if(first != last)
                for(auto tmp = next(first); tmp != last; ++tmp)
                    if(invoke(pred, invoke(proj, *first), invoke(proj, *tmp)))
                        first = tmp;
            return first;
        }

        /// \overload
        template<typename Rng, typename C = less, typename P = identity>
        auto RANGES_FUN_NIEBLOID(max_element)(Rng && rng, C pred = C{}, P proj = P{})
            ->CPP_ret(safe_iterator_t<Rng>)( //
                requires forward_range<Rng> &&
                indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(max_element)

    namespace cpp20
    {
        using ranges::max_element;
    }
    /// @}
} // namespace ranges

#endif // include guard
