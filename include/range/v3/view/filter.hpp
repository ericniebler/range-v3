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
            /// Given a source range and a unary predicate, filter the elements
            /// that satisfy the predicate.
            struct filter_fn
            {
                template<typename Rng, typename Pred>
                remove_if_view<all_t<Rng>, logical_negate<Pred>>
                operator()(Rng && rng, Pred pred) const
                {
                    CONCEPT_ASSERT(Range<Rng>());
                    CONCEPT_ASSERT(IndirectPredicate<Pred, iterator_t<Rng>>());
                    return {all(static_cast<Rng&&>(rng)), not_fn(std::move(pred))};
                }
                template<typename Pred>
                auto operator()(Pred pred) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1,
                        protect(std::move(pred)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1,
                        protect(std::move(pred))));
                }
            };

            /// \relates filter_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(filter_fn, filter)
        }
    }
}

#endif
