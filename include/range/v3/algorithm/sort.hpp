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
#ifndef RANGES_V3_ALGORITHM_SORT_HPP
#define RANGES_V3_ALGORITHM_SORT_HPP

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
        struct sorter : bindable<sorter>,
                        pipeable<sorter>
        {
            /// \brief template function sort
            ///
            /// range-based version of the sort std algorithm
            ///
            /// \pre RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange invoke(sorter, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::sort(ranges::begin(rng), ranges::end(rng));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange invoke(sorter, RandomAccessRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::sort(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto invoke(sorter sort, BinaryPredicate pred)
                -> decltype(sort(std::placeholders::_1, detail::move(pred)))
            {
                return sort(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR sorter sort {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
