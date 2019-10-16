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

#ifndef RANGES_V3_ACTION_TAKE_HPP
#define RANGES_V3_ACTION_TAKE_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct take_fn
        {
            template<typename Int>
            constexpr auto CPP_fun(operator())(Int n)(const //
                                                      requires detail::integer_like_<Int>)
            {
                return make_action_closure(bind_back(take_fn{}, n));
            }

            template<typename Rng>
            auto operator()(Rng && rng, range_difference_t<Rng> n) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> &&
                        erasable_range<Rng &, iterator_t<Rng>, sentinel_t<Rng>>)
            {
                RANGES_EXPECT(n >= 0);
                ranges::actions::erase(
                    rng, ranges::next(begin(rng), n, end(rng)), end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::take_fn
        RANGES_INLINE_VARIABLE(take_fn, take)
    } // namespace actions
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
