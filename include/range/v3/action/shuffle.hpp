/// \file
// Range v3 library
//
//  Copyright Filip Matzner 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_SHUFFLE_HPP
#define RANGES_V3_ACTION_SHUFFLE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace action
    {
        struct shuffle_fn
        {
        private:
            friend action_access;
            template<typename Gen>
            static auto CPP_fun(bind)(shuffle_fn shuffle, Gen &&gen)(
                requires UniformRandomNumberGenerator<Gen>)
            {
                return std::bind(shuffle, std::placeholders::_1,
                    bind_forward<Gen>(gen));
            }
        public:
            CPP_template(typename Rng, typename Gen)(
                requires RandomAccessRange<Rng> &&
                    Permutable<iterator_t<Rng>> &&
                    UniformRandomNumberGenerator<Gen> &&
                    ConvertibleTo<
                        invoke_result_t<Gen &>,
                        range_difference_t<Rng>>)
            Rng operator()(Rng &&rng, Gen &&gen) const
            {
                ranges::shuffle(rng, static_cast<Gen &&>(gen));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates shuffle_fn
        /// \sa `action`
        RANGES_INLINE_VARIABLE(action<shuffle_fn>, shuffle)
    }
    /// @}
}

#endif
