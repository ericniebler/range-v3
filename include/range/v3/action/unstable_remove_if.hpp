/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP
#define RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP

#include <utility>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/unstable_remove_if.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct unstable_remove_if_fn
        {
        private:
            friend action_access;
            template<typename C, typename P = identity>
            static auto CPP_fun(bind)(unstable_remove_if_fn unstable_remove_if, C pred,
                                      P proj = P{})( //
                requires(!range<C>))
            {
                return bind_back(unstable_remove_if, std::move(pred), std::move(proj));
            }

        public:
            template<typename Rng, typename C, typename P = identity>
            auto operator()(Rng && rng, C pred, P proj = P{}) const -> CPP_ret(Rng)( //
                requires bidirectional_range<Rng> && common_range<Rng> &&
                    permutable<iterator_t<Rng>> &&
                        indirect_unary_predicate<C, projected<iterator_t<Rng>, P>> &&
                            erasable_range<Rng, iterator_t<Rng>, iterator_t<Rng>>)
            {
                auto it = ranges::unstable_remove_if(ranges::begin(rng),
                                                     ranges::end(rng),
                                                     std::move(pred),
                                                     std::move(proj));
                ranges::erase(rng, it, ranges::end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \sa `action`
        /// \sa `actions::unstable_remove_if_fn`
        RANGES_INLINE_VARIABLE(action<unstable_remove_if_fn>, unstable_remove_if)
    } // namespace actions
    /// @}
} // namespace ranges

#endif // RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP
