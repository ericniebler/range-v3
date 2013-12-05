//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_SEARCH_HPP
#define RANGES_V3_ALGORITHM_SEARCH_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct searcher : bindable<searcher>
        {
            /// \brief template function search
            ///
            /// range-based version of the search std algorithm
            ///
            /// \pre ForwardRange1 is a model of the ForwardRange concept
            /// \pre ForwardRange2 is a model of the ForwardRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange1, typename ForwardRange2>
            static range_iterator_t<ForwardRange1>
            invoke(searcher, ForwardRange1 && rng1, ForwardRange2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2 const>());
                return std::search(ranges::begin(rng1), ranges::end(rng1),
                                   ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename ForwardRange1, typename ForwardRange2, typename BinaryPredicate>
            static range_iterator_t<ForwardRange1>
            invoke(searcher, ForwardRange1 && rng1, ForwardRange2 const & rng2, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2 const>());
                return std::search(ranges::begin(rng1), ranges::end(rng1),
                                   ranges::begin(rng2), ranges::end(rng2),
                                   ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR searcher search{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
