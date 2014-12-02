// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/view/remove_if.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            /// \cond
            struct filter_fn
            {
                template<typename Rng, typename Pred>
                remove_if_view<Rng, logical_negate<Pred>>
                operator()(Rng && rng, Pred pred) const
                {
                    CONCEPT_ASSERT(Iterable<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<Pred, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), not_(std::move(pred))};
                }
                template<typename Pred>
                auto operator()(Pred pred) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred))));
                }
            };

            RANGES_DEPRECATED("Please switch to view::remove_if")
            /// \sa `filter_fn`
            /// \ingroup group-views
            constexpr filter_fn filter{};
            /// \endcond
        }
    }
}

#endif
