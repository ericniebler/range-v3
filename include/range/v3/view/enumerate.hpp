/// \file
// Range v3 library
//
//  Copyright Casey Carter 2018-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_ENUMERATE_HPP
#define RANGES_V3_VIEW_ENUMERATE_HPP

#include <range/v3/core.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            /// Lazily pairs each element in a source range with
            /// its corresponding index.
            struct enumerate_fn
            {
                template<typename Rng,
                    CONCEPT_REQUIRES_(ViewableRange<Rng>())>
                auto operator()(Rng &&rng) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    zip(iota(range_difference_type_t<Rng>{}),
                        all(static_cast<Rng &&>(rng)))
                )
            };

            /// \relates enumerate_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<enumerate_fn>, enumerate)
        } // namespace view
        /// @}
    } // namespace v3
} // namespace ranges

#endif
