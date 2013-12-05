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
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_COPY_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_COPY_HPP

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
        struct partial_sorter_copier : bindable<partial_sorter_copier>
        {
            /// \brief template function partial_sort_copy
            ///
            /// range-based version of the partial_sort_copy std algorithm
            ///
            /// \pre InputRange is a model of the InputRange concept
            /// \pre RandomAccessRange is a model of the Mutable_RandomAccessRangeConcept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange, typename RandomAccessRange>
            static range_iterator_t<RandomAccessRange>
            invoke(partial_sorter_copier, InputRange && rng1, RandomAccessRange && rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::partial_sort_copy(ranges::begin(rng1), ranges::end(rng1),
                                              ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange, typename RandomAccessRange, typename BinaryPredicate>
            static range_iterator_t<RandomAccessRange>
            invoke(partial_sorter_copier, InputRange && rng1, RandomAccessRange && rng2,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::partial_sort_copy(ranges::begin(rng1), ranges::end(rng1),
                                              ranges::begin(rng2), ranges::end(rng2),
                                              ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR partial_sorter_copier partial_sort_copy {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
