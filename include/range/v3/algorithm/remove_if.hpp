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
#ifndef RANGES_V3_ALGORITHM_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_IF_HPP

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/find_if.hpp>
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

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(remove_if)

        /// \brief function template \c remove_if
        CPP_template(typename I, typename S, typename C, typename P = identity)( //
            requires permutable<I> && sentinel_for<S, I>  && //
            indirect_unary_predicate<C, projected<I, P>>) //
        auto RANGES_FUNC(remove_if)(I first, S last, C pred, P proj = P{}) //
            -> I
        {
            first = find_if(std::move(first), last, std::ref(pred), std::ref(proj));
            if(first != last)
            {
                for(I i = next(first); i != last; ++i)
                {
                    if(!(invoke(pred, invoke(proj, *i))))
                    {
                        *first = iter_move(i);
                        ++first;
                    }
                }
            }
            return first;
        }

        /// \overload
        CPP_template(typename Rng, typename C, typename P = identity)( //
            requires forward_range<Rng> && permutable<iterator_t<Rng>>  && //
            indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>) //
        auto RANGES_FUNC(remove_if)(Rng && rng, C pred, P proj = P{}) //
            -> safe_iterator_t<Rng>
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_FUNC_END(remove_if)

    namespace cpp20
    {
        using ranges::remove_if;
    }
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
