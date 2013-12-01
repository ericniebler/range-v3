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
#ifndef RANGES_V3_ALGORITHM_PARTITION_HPP
#define RANGES_V3_ALGORITHM_PARTITION_HPP

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
        struct partitioner : bindable<partitioner>
        {
            /// \brief template function partition
            ///
            /// range-based version of the partition std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename UnaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(partitioner, ForwardRange && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::partition(ranges::begin(rng), ranges::end(rng), detail::move(pred));
            }

            /// \overload
            template<typename UnaryPredicate>
            static auto invoke(partitioner partition, UnaryPredicate pred)
                -> decltype(partition(std::placeholders::_1, detail::move(pred)))
            {
                return partition(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR partitioner partition {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
