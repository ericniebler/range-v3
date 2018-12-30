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

#ifndef RANGES_V3_VIEW_FILTER_HPP
#define RANGES_V3_VIEW_FILTER_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/remove_if.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            /// Given a source range, unary predicate, and optional projection,
            /// present a view of the elements that satisfy the predicate.
            using filter_fn = remove_if_fn_<not_fn_fn>;

            /// \relates filter_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<filter_fn>, filter)
        }
    }
}

#endif
