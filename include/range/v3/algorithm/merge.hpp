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
#ifndef RANGES_V3_ALGORITHM_MERGE_HPP
#define RANGES_V3_ALGORITHM_MERGE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct merger : bindable<merger>
        {
            /// \brief template function merge
            ///
            /// range-based version of the merge std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2, typename OutputIterator>
            static OutputIterator
            invoke(merger, InputRange1 && rng1, InputRange2 && rng2, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<InputRange1>,
                                                          range_reference_t<InputRange2>> ());
                return std::merge(ranges::begin(rng1), ranges::end(rng1),
                                  ranges::begin(rng2), ranges::end(rng2), detail::move(out));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename OutputIterator,
                     typename BinaryPredicate>
            static OutputIterator
            invoke(merger, InputRange1 && rng1, InputRange2 && rng2, OutputIterator out,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputRange1>,
                                                       range_reference_t<InputRange2>>());
                return std::merge(ranges::begin(rng1), ranges::end(rng1),
                                  ranges::begin(rng2), ranges::end(rng2),
                                  detail::move(out),
                                  ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR merger merge {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
