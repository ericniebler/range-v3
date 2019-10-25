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
#ifndef RANGES_V3_ACTION_ADJACENT_REMOVE_IF_HPP
#define RANGES_V3_ACTION_ADJACENT_REMOVE_IF_HPP

#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/adjacent_remove_if.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct adjacent_remove_if_fn
        {
            template<typename Pred, typename Proj = identity>
            constexpr auto CPP_fun(operator())(Pred pred,
                                               Proj proj = {})(const //
                                                               requires(!range<Pred>))
            {
                return make_action_closure(
                    bind_back(adjacent_remove_if_fn{}, std::move(pred), std::move(proj)));
            }

            template<typename Rng, typename Pred, typename Proj = identity>
            auto operator()(Rng && rng, Pred pred, Proj proj = {}) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> &&
                        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> &&
                            indirect_relation<Pred, projected<iterator_t<Rng>, Proj>> &&
                                permutable<iterator_t<Rng>>)
            {
                auto i = adjacent_remove_if(rng, std::move(pred), std::move(proj));
                erase(rng, std::move(i), end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::adjacent_remove_if_fn
        RANGES_INLINE_VARIABLE(adjacent_remove_if_fn, adjacent_remove_if)
    } // namespace actions
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif // RANGES_V3_ACTION_ADJACENT_REMOVE_IF_HPP
