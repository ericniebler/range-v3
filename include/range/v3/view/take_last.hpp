/// \file
// Range v3 library
//
//  Copyright Barry Revzin 2019-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_TAKE_LAST_HPP
#define RANGES_V3_VIEW_TAKE_LAST_HPP

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/drop_exactly.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    namespace views
    {
        struct take_last_base_fn
        {
            template<typename Rng>
            auto CPP_fun(operator())(Rng && rng, range_difference_t<Rng> n)(
                const requires viewable_range<Rng> && sized_range<Rng>)
            {
                auto sz = ranges::distance(rng);
                return drop_exactly(static_cast<Rng &&>(rng), sz > n ? sz - n : 0);
            }
        };

        struct take_last_fn : take_last_base_fn
        {
            using take_last_base_fn::operator();

            template<typename Int>
            constexpr auto CPP_fun(operator())(Int n)(const //
                                                      requires detail::integer_like_<Int>)
            {
                return make_view_closure(bind_back(take_last_base_fn{}, n));
            }
        };

        /// \relates take_last_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(take_last_fn, take_last)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
