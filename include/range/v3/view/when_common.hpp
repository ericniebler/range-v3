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

#ifndef RANGES_V3_VIEW_WHEN_COMMON_HPP
#define RANGES_V3_VIEW_WHEN_COMMON_HPP

#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/semiregular.hpp>

namespace ranges
{
    namespace detail
    {
        template<typename Pred>
        struct predicate_pred
        {
            semiregular_t<Pred> pred_;

            template<typename I, typename S>
            auto operator()(I cur, S end) const -> CPP_ret(std::pair<bool, I>)( //
                requires Sentinel<S, I>)
            {
                auto where = ranges::find_if_not(cur, end, std::ref(pred_));
                return {cur != where, where};
            }
        };
    } // namespace detail
} // namespace ranges

#endif

