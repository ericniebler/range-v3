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

#include <range/v3/view/drop_exactly.hpp>

namespace ranges
{
    namespace view
    {
        struct take_last_fn
        {
        private:
            friend view_access;
            template<typename Int>
            static auto CPP_fun(bind)(take_last_fn take_last, Int n)( //
                requires Integral<Int>)
            {
                return make_pipeable(std::bind(take_last, std::placeholders::_1, n));
            }

        public:
            template<typename Rng>
            auto CPP_fun(operator())(Rng && rng, range_difference_t<Rng> n)(
                const requires ViewableRange<Rng> && InputRange<Rng> && SizedRange<Rng>)
            {
                auto sz = ranges::distance(rng);
                return drop_exactly(static_cast<Rng &&>(rng), sz > n ? sz - n : 0);
            }
        };

        /// \relates take_last_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<take_last_fn>, take_last)
    } // namespace view
    /// @}
} // namespace ranges

#endif
