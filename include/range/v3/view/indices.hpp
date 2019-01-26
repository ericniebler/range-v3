/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_INDICES_HPP
#define RANGES_V3_VIEW_INDICES_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/iota.hpp>

namespace ranges
{
    namespace view
    {
        /// Half-open range of indices: [from, to).
        struct indices_fn
          : iota_view<std::ptrdiff_t>
        {
            indices_fn() = default;

            template<typename Val>
            auto CPP_fun(operator())(Val from, Val to) (const
                requires Integral<Val>)
            {
                return detail::take_exactly_view_<iota_view<Val>, true>
                    {iota_view<Val>{from}, detail::ints_open_distance_(from, to)};
            }
            template<typename Val, typename Self = indices_fn>
            auto CPP_fun(operator())(Val to) (const
                requires Integral<Val>)
            {
                return Self{}(Val(), to);
            }
        };

        /// Inclusive range of indices: [from, to].
        struct closed_indices_fn
          : iota_view<std::ptrdiff_t>
        {
            template<typename Val>
            auto CPP_fun(operator())(Val from, Val to) (const
                requires Integral<Val>)
            {
                return detail::take_exactly_view_<iota_view<Val>, true>
                    {iota_view<Val>{from}, detail::ints_closed_distance_(from, to)};
            }
            template<typename Val, typename Self = closed_indices_fn>
            auto CPP_fun(operator())(Val to) (const
                requires Integral<Val>)
            {
                return Self{}(Val(), to);
            }
        };

        /// \relates indices_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(indices_fn, indices)

        /// \relates closed_indices_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(closed_indices_fn, closed_indices)

    }  // namespace view
}

#endif  // RANGES_V3_VIEW_INDICES_HPP
