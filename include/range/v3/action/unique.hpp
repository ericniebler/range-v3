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
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace action
    {
        struct unique_fn
        {
        private:
            friend action_access;
            template<typename C, typename P = identity>
            static auto CPP_fun(bind)(unique_fn unique, C pred, P proj = P{})(
                requires (not Range<C>))
            {
                return std::bind(unique, std::placeholders::_1, protect(std::move(pred)),
                    protect(std::move(proj)));
            }
        public:
            CPP_template(typename Rng, typename C = equal_to, typename P = identity)(
                requires ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                    Sortable<iterator_t<Rng>, C, P>)
            Rng operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                auto it = ranges::unique(rng, std::move(pred), std::move(proj));
                ranges::erase(rng, it, end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates unique_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<unique_fn>, unique)
    }
    /// @}
}

#endif
