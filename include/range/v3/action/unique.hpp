/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_UNIQUE_HPP
#define RANGES_V3_ACTION_UNIQUE_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct unique_fn
        {
            template<typename C, typename P = identity>
            constexpr auto CPP_fun(operator())(C pred, P proj = P{})(const //
                                                                     requires(!range<C>))
            {
                return make_action_closure(
                    bind_back(unique_fn{}, std::move(pred), std::move(proj)));
            }

            template<typename Rng, typename C = equal_to, typename P = identity>
            auto operator()(Rng && rng, C pred = C{}, P proj = P{}) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> &&
                        erasable_range<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                            sortable<iterator_t<Rng>, C, P>)
            {
                auto it = ranges::unique(rng, std::move(pred), std::move(proj));
                ranges::erase(rng, it, end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates detail::unique_fn
        /// \sa action_closure
        RANGES_INLINE_VARIABLE(action_closure<unique_fn>, unique)
    } // namespace actions
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
