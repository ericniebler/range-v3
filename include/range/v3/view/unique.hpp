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
            CPP_def
            (
                template(typename Rng)
                concept UniqueViewConcept,
                    ForwardRange<Rng> &&
                    EqualityComparable<range_value_t<Rng>>
            );

            struct unique_fn
            {
                template<typename Rng>
                auto operator()(Rng &&rng) const ->
                    CPP_ret(unique_view<all_t<Rng>>)(
                        requires UniqueViewConcept<Rng>)
                {
                    return {all(static_cast<Rng &&>(rng)), not_equal_to{}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng>
                auto operator()(Rng &&) const ->
                    CPP_ret(void)(
                        requires not UniqueViewConcept<Rng>)
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which view::unique operates must be a model the "
                        "ForwardRange concept.");
                    CPP_assert_msg(EqualityComparable<range_value_t<Rng>>,
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
