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
#ifndef RANGES_V3_ALGORITHM_NTH_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_NTH_ELEMENT_HPP

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
        struct nth_element_finder : bindable<nth_element_finder>
        {
            /// \brief template function nth_element
            ///
            /// range-based version of the nth_element std algorithm
            ///
            /// \pre RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange
            invoke(nth_element_finder, RandomAccessRange && rng,
                   range_iterator_t<RandomAccessRange> nth)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<RandomAccessRange>>());
                std::nth_element(ranges::begin(rng), std::move(nth), ranges::end(rng));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange
            invoke(nth_element_finder, RandomAccessRange && rng,
                   range_iterator_t<RandomAccessRange> nth, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<RandomAccessRange>,
                                                       range_reference_t<RandomAccessRange>>());
                std::nth_element(ranges::begin(rng), std::move(nth), ranges::end(rng),
                                 ranges::make_invokable(std::move(pred)));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            /// for rng | nth_element(it)
            template<typename RandomAccessIterator,
                CONCEPT_REQUIRES(ranges::Iterator<RandomAccessIterator>())>
            static auto
            invoke(nth_element_finder nth_element, RandomAccessIterator nth)
                -> decltype(nth_element(std::placeholders::_1, std::move(nth)))
            {
                CONCEPT_ASSERT(ranges::RandomAccessIterator<RandomAccessIterator>());
                return nth_element(std::placeholders::_1, std::move(nth));
            }

            /// \overload
            /// for rng | nth_element(it, pred)
            template<typename RandomAccessIterator, typename BinaryPredicate,
                CONCEPT_REQUIRES(ranges::Iterator<RandomAccessIterator>())>
            static auto
            invoke(nth_element_finder nth_element, RandomAccessIterator nth, BinaryPredicate pred)
            -> decltype(nth_element(std::placeholders::_1, std::move(nth), std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::RandomAccessIterator<RandomAccessIterator>());
                return nth_element(std::placeholders::_1, std::move(nth), std::move(pred));
            }
        };

        RANGES_CONSTEXPR nth_element_finder nth_element {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
