/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_EMPTY_HPP
#define RANGES_V3_EMPTY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-core
        struct empty_fn
        {
            /// \return `begin(rng) == end(rng)`
            template<typename Rng,
                CONCEPT_REQUIRES_(Range<Rng>())>
            RANGES_CXX14_CONSTEXPR
            bool operator()(Rng &&rng) const
            {
                return begin(rng) == end(rng);
            }
        };

        /// \ingroup group-core
        /// \sa `empty_fn`
        RANGES_INLINE_VARIABLE(empty_fn, empty)
    }
}

#endif
