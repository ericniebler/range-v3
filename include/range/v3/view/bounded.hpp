/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_BOUNDED_HPP
#define RANGES_V3_VIEW_BOUNDED_HPP

#include <range/v3/view/common.hpp>

namespace ranges
{
    /// \cond
    template<typename Rng>
    using bounded_view
        RANGES_DEPRECATED("The name bounded_view is deprecated. "
                          "Please use common_view instead.") =
            common_view<Rng>;
    /// \endcond

    namespace view
    {
        /// \cond
        inline namespace
        {
            RANGES_DEPRECATED("The name view::bounded is deprecated. "
                              "Please use view::common instead.")
            constexpr auto &bounded = common;
        }
        /// \endcond

        /// \cond
        template<typename Rng>
        using bounded_t
            RANGES_DEPRECATED("The name view::bounded_t is deprecated. "
                              "Please use view::common_t instead.") =
                decltype(common(std::declval<Rng>()));
        /// \endcond
    }
    /// @}
}

#endif
