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
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct partial_sorter : bindable<partial_sorter>
        {
            /// \brief template function partial_sort
            ///
            /// range-based version of the partial_sort std algorithm
            ///
            /// \pre RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange
            invoke(partial_sorter, RandomAccessRange && rng,
                range_iterator_t<RandomAccessRange> middle)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<RandomAccessRange>>());
                std::partial_sort(ranges::begin(rng), std::move(middle), ranges::end(rng));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange
            invoke(partial_sorter, RandomAccessRange && rng,
                range_iterator_t<RandomAccessRange> middle, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<RandomAccessRange>,
                                                       range_reference_t<RandomAccessRange>>());
                std::partial_sort(ranges::begin(rng), std::move(middle), ranges::end(rng),
                                  ranges::make_invokable(std::move(pred)));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            /// for rng | partial_sort(it)
            template<typename RandomAccessIterator,
                CONCEPT_REQUIRES(ranges::Iterator<RandomAccessIterator>())>
            static auto
            invoke(partial_sorter partial_sort, RandomAccessIterator nth) ->
                decltype(partial_sort.move_bind(std::placeholders::_1, std::move(nth)))
            {
                CONCEPT_ASSERT(ranges::RandomAccessIterator<RandomAccessIterator>());
                return partial_sort.move_bind(std::placeholders::_1, std::move(nth));
            }

            /// \overload
            /// for rng | partial_sort(it, pred)
            template<typename RandomAccessIterator, typename BinaryPredicate,
                CONCEPT_REQUIRES(ranges::Iterator<RandomAccessIterator>())>
            static auto
            invoke(partial_sorter partial_sort, RandomAccessIterator nth, BinaryPredicate pred) ->
                decltype(partial_sort.move_bind(std::placeholders::_1, std::move(nth), std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::RandomAccessIterator<RandomAccessIterator>());
                return partial_sort.move_bind(std::placeholders::_1, std::move(nth), std::move(pred));
            }
        };

        RANGES_CONSTEXPR partial_sorter partial_sort {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
