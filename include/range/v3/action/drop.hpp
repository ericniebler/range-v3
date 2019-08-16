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

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct drop_fn
        {
        private:
            friend action_access;
            template<typename Int>
            static auto CPP_fun(bind)(drop_fn drop, Int n)( //
                requires integral<Int>)
            {
                return bind_back(drop, n);
            }

        public:
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

        /// \ingroup group-actions
        /// \relates drop_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<drop_fn>, drop)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
