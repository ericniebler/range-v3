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
#ifndef RANGES_V3_ALGORITHM_REPLACE_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_IF_HPP

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

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
    RANGES_BEGIN_NIEBLOID(replace_if)

        /// \brief function template \c replace_if
        template<typename I, typename S, typename C, typename T, typename P = identity>
        auto RANGES_FUN_NIEBLOID(replace_if)(
            I first, S last, C pred, T const & new_value, P proj = P{}) //
            ->CPP_ret(I)(                                               //
                requires input_iterator<I> && sentinel_for<S, I> &&
                indirect_unary_predicate<C, projected<I, P>> && writable<I, T const &>)
        {
            for(; first != last; ++first)
                if(invoke(pred, invoke(proj, *first)))
                    *first = new_value;
            return first;
        }

        /// \overload
        template<typename Rng, typename C, typename T, typename P = identity>
        auto RANGES_FUN_NIEBLOID(replace_if)(
            Rng && rng, C pred, T const & new_value, P proj = P{}) //
            ->CPP_ret(safe_iterator_t<Rng>)(                       //
                requires input_range<Rng> &&
                indirect_unary_predicate<C, projected<iterator_t<Rng>, P>> &&
                writable<iterator_t<Rng>, T const &>)
        {
            return (*this)(
                begin(rng), end(rng), std::move(pred), new_value, std::move(proj));
        }

    RANGES_END_NIEBLOID(replace_if)

    namespace cpp20
    {
        using ranges::replace_if;
    }
    /// @}
} // namespace ranges

#endif // include guard
