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
#ifndef RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct min_element_finder : bindable<min_element_finder>,
                                    pipeable<min_element_finder>
        {
            /// \brief template function min_element
            ///
            /// range-based version of the min_element std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange,
                CONCEPT_REQUIRES(ranges::Range<ForwardRange>())>
            static range_iterator_t<ForwardRange>
            invoke(min_element_finder, ForwardRange && rng)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<ForwardRange>>());
                return std::min_element(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardRange, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(min_element_finder, ForwardRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardRange>,
                                                       range_reference_t<ForwardRange>>());
                return std::min_element(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | max_element(pred)
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto
            invoke(min_element_finder min_element, BinaryPredicate pred)
                -> decltype(min_element(std::placeholders::_1, std::move(pred)))
            {
                return min_element(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR min_element_finder min_element {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
