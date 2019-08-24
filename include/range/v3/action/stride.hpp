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

#ifndef RANGES_V3_ACTION_STRIDE_HPP
#define RANGES_V3_ACTION_STRIDE_HPP

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
        struct stride_fn
        {
        private:
            friend action_access;
            template<typename D>
            static auto CPP_fun(bind)(stride_fn stride, D step)( //
                requires integral<D>)
            {
                return bind_back(stride, step);
            }

        public:
            template<typename Rng, typename D = range_difference_t<Rng>>
            auto operator()(Rng && rng, range_difference_t<Rng> const step) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> &&
                        erasable_range<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                            permutable<iterator_t<Rng>>)
            {
                using I = iterator_t<Rng>;
                using S = sentinel_t<Rng>;
                RANGES_EXPECT(0 < step);
                if(1 < step)
                {
                    I first = ranges::begin(rng);
                    S const last = ranges::end(rng);
                    if(first != last)
                    {
                        for(I i = ranges::next(++first, step - 1, last); i != last;
                            advance(i, step, last), ++first)
                        {
                            *first = iter_move(i);
                        }
                    }
                    ranges::actions::erase(rng, first, last);
                }
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates stride_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<stride_fn>, stride)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
