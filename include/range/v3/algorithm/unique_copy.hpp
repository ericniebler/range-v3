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
#ifndef RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP

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
        struct uniquer_copier : bindable<uniquer_copier>
        {
            /// \brief template function unique_copy
            ///
            /// range-based version of the unique_copy std algorithm
            ///
            /// \pre InputRange is a model of the InputRange concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange, typename OutputIterator,
                CONCEPT_REQUIRES(ranges::Range<InputRange>())>
            static OutputIterator
            invoke(uniquer_copier, InputRange && rng, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<InputRange>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputRange>>());
                CONCEPT_ASSERT(
                    (ranges::ForwardRange<InputRange>() &&
                     ranges::ForwardIterator<OutputIterator>()) ||
                    (ranges::CopyConstructible<range_value_t<InputRange>>() &&
                     ranges::CopyAssignable<range_value_t<InputRange>>()));
                return std::unique_copy(ranges::begin(rng), ranges::end(rng),
                    detail::move(out));
            }

            /// \overload
            template<typename InputRange, typename OutputIterator, typename BinaryPredicate>
            static OutputIterator
            invoke(uniquer_copier, InputRange && rng, OutputIterator out,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputRange>,
                                                       range_reference_t<InputRange>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputRange>>());
                CONCEPT_ASSERT(
                    (ranges::ForwardRange<InputRange>() &&
                     ranges::ForwardIterator<OutputIterator>()) ||
                    (ranges::CopyConstructible<range_value_t<InputRange>>() &&
                     ranges::CopyAssignable<range_value_t<InputRange>>()));
                return std::unique_copy(ranges::begin(rng), ranges::end(rng),
                    detail::move(out), ranges::make_invokable(detail::move(pred)));
            }

            /// \overload
            /// for rng | unique_copy(out)
            template<typename OutputIterator>
            static auto
            invoke(uniquer_copier unique_copy, OutputIterator out)
                -> decltype(unique_copy(std::placeholders::_1, detail::move(out)))
            {
                return unique_copy(std::placeholders::_1, detail::move(out));
            }

            /// \overload
            /// for rng | unique_copy(out, pred)
            template<typename OutputIterator, typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<OutputIterator>())>
            static auto
            invoke(uniquer_copier unique_copy, OutputIterator out, BinaryPredicate pred)
                -> decltype(unique_copy(std::placeholders::_1, detail::move(out), detail::move(pred)))
            {
                return unique_copy(std::placeholders::_1, detail::move(out), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR uniquer_copier unique_copy {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
