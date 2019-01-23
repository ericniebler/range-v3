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

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace action
    {
        struct stride_fn
        {
        private:
            friend action_access;
            template<typename D>
            static auto CPP_fun(bind)(stride_fn stride, D step)(
                requires Integral<D>)
            {
                return std::bind(stride, std::placeholders::_1, step);
            }
        public:
            CPP_template(typename Rng, typename D = range_difference_t<Rng>)(
                requires ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                    Permutable<iterator_t<Rng>>)
            Rng operator()(Rng &&rng, range_difference_t<Rng> const step) const
            {
                using I = iterator_t<Rng>;
                using S = sentinel_t<Rng>;
                RANGES_EXPECT(0 < step);
                if(1 < step)
                {
                    I begin = ranges::begin(rng);
                    S const end = ranges::end(rng);
                    if(begin != end)
                    {
                        for(I i = ranges::next(++begin, step-1, end); i != end;
                            advance(i, step, end), ++begin)
                        {
                            *begin = iter_move(i);
                        }
                    }
                    ranges::action::erase(rng, begin, end);
                }
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates stride_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<stride_fn>, stride)
    }
    /// @}
}

#endif
