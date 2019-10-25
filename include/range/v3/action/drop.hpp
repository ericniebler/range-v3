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

#ifndef RANGES_V3_ACTION_DROP_HPP
#define RANGES_V3_ACTION_DROP_HPP

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
        struct drop_fn
        {
            template<typename Int>
            constexpr auto CPP_fun(operator())(Int n)(const //
                                                      requires detail::integer_like_<Int>)
            {
                RANGES_EXPECT(n >= Int(0));
                return make_action_closure(bind_back(drop_fn{}, n));
            }

            template<typename Rng>
            auto operator()(Rng && rng, range_difference_t<Rng> n) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> &&
                        erasable_range<Rng &, iterator_t<Rng>, iterator_t<Rng>>)
            {
                RANGES_EXPECT(n >= 0);
                ranges::actions::erase(
                    rng, begin(rng), ranges::next(begin(rng), n, end(rng)));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::drop_fn
        RANGES_INLINE_VARIABLE(drop_fn, drop)
    } // namespace actions
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
