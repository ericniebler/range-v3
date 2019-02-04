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

#ifndef RANGES_V3_ACTION_DROP_WHILE_HPP
#define RANGES_V3_ACTION_DROP_WHILE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace action
    {
        struct drop_while_fn
        {
        private:
            friend action_access;
            CPP_template(typename Fun)(
            requires !Range<Fun>)
            static auto bind(drop_while_fn drop_while, Fun fun)
            {
                return std::bind(drop_while, std::placeholders::_1, std::move(fun));
            }
        public:
            CPP_template(typename Rng, typename Fun)(
                requires ForwardRange<Rng> &&
                    IndirectUnaryPredicate<Fun, iterator_t<Rng>> &&
                    ErasableRange<Rng &, iterator_t<Rng>, iterator_t<Rng>>)
            Rng operator()(Rng &&rng, Fun fun) const
            {
                ranges::action::erase(rng, begin(rng), find_if_not(begin(rng), end(rng),
                    std::move(fun)));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates drop_while_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<drop_while_fn>, drop_while)
    }
    /// @}
}

#endif
