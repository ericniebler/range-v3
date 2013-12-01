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
#ifndef RANGES_V3_ALGORITHM_STABLE_PARTITION_HPP
#define RANGES_V3_ALGORITHM_STABLE_PARTITION_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct stable_partitioner : bindable<stable_partitioner>
        {
            /// \brief template function stable_partition
            ///
            /// range-based version of the stable_partition std algorithm
            ///
            /// \pre BidirectionalRange is a model of the BidirectionalRange concept
            /// \pre UnaryPredicate is a model of the UnaryPredicate concept
            template<typename BidirectionalRange, typename UnaryPredicate>
            static range_iterator_t<BidirectionalRange>
            invoke(stable_partitioner, BidirectionalRange && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                return std::stable_partition(ranges::begin(rng), ranges::end(rng),
                    detail::move(pred));
            }

            /// \overload
            template<typename UnaryPredicate>
            static auto invoke(stable_partitioner stable_partition, UnaryPredicate pred)
                -> decltype(stable_partition(std::placeholders::_1, detail::move(pred)))
            {
                return stable_partition(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR stable_partitioner stable_partition{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
