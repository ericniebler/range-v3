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
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take_exactly.hpp>

namespace ranges
{
    namespace views
    {
        /// Half-open range of indices: [from, to).
        struct indices_fn : iota_view<std::size_t>
        {
            indices_fn() = default;

            template<typename Val>
            auto operator()(Val to) const -> CPP_ret(iota_view<Val, Val>)( //
                requires integral<Val>)
            {
                return {Val(), to};
            }
            template<typename Val>
            auto operator()(Val from, Val to) const -> CPP_ret(iota_view<Val, Val>)( //
                requires integral<Val>)
            {
                return {from, to};
            }
        };

        /// Inclusive range of indices: [from, to].
        struct closed_indices_fn
        {
            template<typename Val>
            auto operator()(Val to) const -> CPP_ret(closed_iota_view<Val>)( //
                requires integral<Val>)
            {
                return {Val(), to};
            }
            template<typename Val>
            auto operator()(Val from, Val to) const -> CPP_ret(closed_iota_view<Val>)( //
                requires integral<Val>)
            {
                return {from, to};
            }
        };

        /// \relates indices_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(indices_fn, indices)

        /// \relates closed_indices_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(closed_indices_fn, closed_indices)
    } // namespace views
} // namespace ranges

#endif // RANGES_V3_VIEW_INDICES_HPP
