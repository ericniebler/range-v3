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

#ifndef RANGES_V3_VIEW_UNIQUE_HPP
#define RANGES_V3_VIEW_UNIQUE_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace view
    {
        struct unique_fn
        {
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(unique_view<all_t<Rng>>)(
                    requires ViewableRange<Rng> && ForwardRange<Rng> &&
                        EqualityComparable<range_value_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), not_equal_to{}};
            }
        };

        /// \relates unique_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<unique_fn>, unique)
    }
    /// @}
}

#endif
