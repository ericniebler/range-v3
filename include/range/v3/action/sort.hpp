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

#ifndef RANGES_V3_ACTION_SORT_HPP
#define RANGES_V3_ACTION_SORT_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct sort_fn
            {
            private:
                friend action_access;
                template<typename C, typename P = ident, CONCEPT_REQUIRES_(!Iterable<C>())>
                static auto bind(sort_fn sort, C pred, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(sort, std::placeholders::_1, protect(std::move(pred)), protect(std::move(proj)))
                )
            public:
                template<typename Rng, typename C = ordered_less, typename P = ident,
                    typename I = range_iterator_t<Rng>,
                    CONCEPT_REQUIRES_(Iterable<Rng &>() && Sortable<I, C, P>())>
                void operator()(Rng & rng, C pred = C{}, P proj = P{}) const
                {
                    ranges::sort(rng, std::move(pred), std::move(proj));
                }
            };

            /// \ingroup group-actions
            /// \sa `sort_fn`
            /// \sa `action`
            constexpr action<sort_fn> sort{};
        }
        /// @}
    }
}

#endif
