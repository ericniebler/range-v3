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
#ifndef RANGES_V3_ALGORITHM_STABLE_SORT_HPP
#define RANGES_V3_ALGORITHM_STABLE_SORT_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct stable_sorter : bindable<stable_sorter>,
                               pipeable<stable_sorter>
        {
            /// \brief template function stable_sort
            ///
            /// range-based version of the stable_sort std algorithm
            ///
            /// \pre RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange invoke(stable_sorter, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<RandomAccessRange>>());
                std::stable_sort(ranges::begin(rng), ranges::end(rng));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange invoke(stable_sorter, RandomAccessRange && rng,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<RandomAccessRange>,
                                                       range_reference_t<RandomAccessRange>>());
                std::stable_sort(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto invoke(stable_sorter stable_sort, BinaryPredicate pred)
                -> decltype(stable_sort.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return stable_sort.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR stable_sorter stable_sort{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
