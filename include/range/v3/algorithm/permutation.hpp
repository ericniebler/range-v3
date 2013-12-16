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
#ifndef RANGES_V3_ALGORITHM_PERMUTATION_HPP
#define RANGES_V3_ALGORITHM_PERMUTATION_HPP

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
        struct next_permutation_finder : bindable<next_permutation_finder>
        {
            /// \brief template function next_permutation
            ///
            /// range-based version of the next_permutation std algorithm
            ///
            /// \pre BidirectionalRange is a model of the BidirectionalRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename BidirectionalRange>
            static bool invoke(next_permutation_finder, BidirectionalRange && rng)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                return std::next_permutation(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename BidirectionalRange, typename BinaryPredicate>
            static bool invoke(next_permutation_finder, BidirectionalRange && rng,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                return std::next_permutation(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR next_permutation_finder next_permutation {};

        struct prev_permutation_finder : bindable<prev_permutation_finder>
        {
            /// \brief template function prev_permutation
            ///
            /// range-based version of the prev_permutation std algorithm
            ///
            /// \pre BidirectionalRange is a model of the BidirectionalRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename BidirectionalRange>
            static bool invoke(prev_permutation_finder, BidirectionalRange && rng)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                return std::prev_permutation(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename BidirectionalRange, typename BinaryPredicate>
            static bool invoke(prev_permutation_finder, BidirectionalRange && rng,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                return std::prev_permutation(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR prev_permutation_finder prev_permutation{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
