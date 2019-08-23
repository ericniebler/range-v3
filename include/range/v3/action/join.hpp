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

#ifndef RANGES_V3_ACTION_JOIN_HPP
#define RANGES_V3_ACTION_JOIN_HPP

#include <vector>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        template<typename Rng>
        using join_action_value_t_ =
            meta::if_c<(bool)ranges::container<range_value_t<Rng>>, range_value_t<Rng>,
                       std::vector<range_value_t<range_value_t<Rng>>>>;

        struct join_fn
        {
        public:
            template<typename Rng>
            auto operator()(Rng && rng) const -> CPP_ret(join_action_value_t_<Rng>)( //
                requires input_range<Rng> && input_range<range_value_t<Rng>> &&
                    semiregular<join_action_value_t_<Rng>>)
            {
                join_action_value_t_<Rng> ret;
                auto last = ranges::end(rng);
                for(auto it = begin(rng); it != last; ++it)
                    push_back(ret, *it);
                return ret;
            }
        };

        /// \ingroup group-actions
        /// \relates join_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<join_fn>, join)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
