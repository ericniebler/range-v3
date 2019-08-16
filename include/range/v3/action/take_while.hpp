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

#ifndef RANGES_V3_ACTION_TAKE_WHILE_HPP
#define RANGES_V3_ACTION_TAKE_WHILE_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct take_while_fn
        {
        private:
            friend action_access;
            template<typename Fun>
            static auto CPP_fun(bind)(take_while_fn take_while, Fun fun)( //
                requires(!range<Fun>))
            {
                return bind_back(take_while, std::move(fun));
            }

        public:
            template<typename Rng, typename Fun>
            auto operator()(Rng && rng, Fun fun) const -> CPP_ret(Rng)( //
                requires forward_range<Rng> &&
                    erasable_range<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                        indirect_unary_predicate<Fun, iterator_t<Rng>>)
            {
                ranges::actions::erase(
                    rng, find_if_not(begin(rng), end(rng), std::move(fun)), end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates take_while_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<take_while_fn>, take_while)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
