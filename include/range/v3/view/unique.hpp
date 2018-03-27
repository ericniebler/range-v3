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

#ifndef RANGES_V3_VIEW_UNIQUE_HPP
#define RANGES_V3_VIEW_UNIQUE_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct unique_fn
            {
                template<typename Rng>
                using Concept = meta::and_<
                    ForwardRange<Rng>,
                    EqualityComparable<range_value_type_t<Rng>>>;

                template<typename Rng, CONCEPT_REQUIRES_(Concept<Rng>())>
                unique_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return {all(static_cast<Rng&&>(rng)), not_equal_to{}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The object on which view::unique operates must be a model the "
                        "ForwardRange concept.");
                    CONCEPT_ASSERT_MSG(EqualityComparable<range_value_type_t<Rng>>(),
                        "The value type of the range passed to view::unique must be "
                        "EqualityComparable.");
                }
            #endif
            };

            /// \relates unique_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<unique_fn>, unique)
        }
        /// @}
    }
}

#endif
