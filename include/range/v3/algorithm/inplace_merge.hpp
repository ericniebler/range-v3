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
#ifndef RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP
#define RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP

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
        struct inplace_merger : bindable<inplace_merger>
        {
            /// \brief template function \c inplace_merger::operator()
            ///
            /// range-based version of the \c inplace_merge std algorithm
            ///
            /// \pre \c BidirectionalRange is a model of the BidirectionalRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename BidirectionalRange>
            static BidirectionalRange
            invoke(inplace_merger, BidirectionalRange && rng,
                range_iterator_t<BidirectionalRange> middle)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                std::inplace_merge(ranges::begin(rng), detail::move(middle), ranges::end(rng));
                return detail::forward<BidirectionalRange>(rng);
            }

            /// \overload
            template<typename BidirectionalRange, typename BinaryPredicate>
            static BidirectionalRange
            invoke(inplace_merger, BidirectionalRange && rng,
                range_iterator_t<BidirectionalRange> middle, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                std::inplace_merge(ranges::begin(rng), detail::move(middle),
                                   ranges::end(rng), ranges::make_invokable(detail::move(pred)));
                return detail::forward<BidirectionalRange>(rng);
            }
        };

        RANGES_CONSTEXPR inplace_merger inplace_merge {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
