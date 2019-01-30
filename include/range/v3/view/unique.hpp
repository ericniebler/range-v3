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
#include <range/v3/functional/not_fn.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace view
    {
        struct unique_fn
        {
        private:
            friend view_access;
            template<typename C>
            static auto CPP_fun(bind)(unique_fn unique, C pred)(
                requires (!Range<C>))
            {
                return std::bind(unique, std::placeholders::_1, protect(std::move(pred)));
            }

        public:
            template<typename Rng, typename C = equal_to>
            auto operator()(Rng && rng, C pred = {}) const ->
                CPP_ret(adjacent_filter_view<all_t<Rng>, logical_negate<C>>)(
                    requires ViewableRange<Rng> && ForwardRange<Rng> &&
                        IndirectRelation<C, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), not_fn(pred)};
            }
        };

        /// \relates unique_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<unique_fn>, unique)
    }
    /// @}
}

#endif
