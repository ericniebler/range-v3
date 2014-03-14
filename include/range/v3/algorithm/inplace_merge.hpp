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
#ifndef RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP
#define RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct inplace_merger : bindable<inplace_merger>
        {
            /// \brief template function \c inplace_merger::operator()
            ///
            /// range-based version of the \c inplace_merge std algorithm
            ///
            /// \pre \c BidirectionalRange is a model of the BidirectionalRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename BidirectionalRange,
                CONCEPT_REQUIRES_(ranges::Range<BidirectionalRange>())>
            static BidirectionalRange
            invoke(inplace_merger, BidirectionalRange && rng,
                range_iterator_t<BidirectionalRange> middle)
            {
                CONCEPT_ASSERT(ranges::Range<BidirectionalRange>());
                CONCEPT_ASSERT(ranges::BidirectionalIterator<range_iterator_t<BidirectionalRange>>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<BidirectionalRange>>());
                std::inplace_merge(ranges::begin(rng), std::move(middle), ranges::end(rng));
                return std::forward<BidirectionalRange>(rng);
            }

            /// \overload
            template<typename BidirectionalRange, typename BinaryPredicate,
                CONCEPT_REQUIRES_(ranges::Range<BidirectionalRange>())>
            static BidirectionalRange
            invoke(inplace_merger, BidirectionalRange && rng,
                range_iterator_t<BidirectionalRange> middle, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Range<BidirectionalRange>());
                CONCEPT_ASSERT(ranges::BidirectionalIterator<range_iterator_t<BidirectionalRange>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<BidirectionalRange>,
                                                       range_reference_t<BidirectionalRange>>());
                std::inplace_merge(ranges::begin(rng), std::move(middle),
                                   ranges::end(rng), ranges::make_invokable(std::move(pred)));
                return std::forward<BidirectionalRange>(rng);
            }

            /// \overload
            /// for rng | inplace_merge(middle)
            template<typename BidirectionalIterator,
                CONCEPT_REQUIRES_(ranges::Iterator<BidirectionalIterator>())>
            static auto
            invoke(inplace_merger inplace_merge, BidirectionalIterator middle) ->
                decltype(inplace_merge.move_bind(std::placeholders::_1, std::move(middle)))
            {
                CONCEPT_ASSERT(ranges::BidirectionalIterator<BidirectionalIterator>());
                CONCEPT_ASSERT(ranges::LessThanComparable<
                    iterator_reference_t<BidirectionalIterator>>());
                return inplace_merge.move_bind(std::placeholders::_1, std::move(middle));
            }

            /// \overload
            /// for rng | inplace_merge(middle, pred)
            template<typename BidirectionalIterator, typename BinaryPredicate,
                CONCEPT_REQUIRES_(ranges::Iterator<BidirectionalIterator>())>
            static auto
            invoke(inplace_merger inplace_merge, BidirectionalIterator middle, BinaryPredicate pred) ->
                decltype(inplace_merge.move_bind(std::placeholders::_1, std::move(middle),
                                          std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::BidirectionalIterator<BidirectionalIterator>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       iterator_reference_t<BidirectionalIterator>,
                                                       iterator_reference_t<BidirectionalIterator>>());
                return inplace_merge.move_bind(std::placeholders::_1, std::move(middle),
                                     std::move(pred));
            }
        };

        RANGES_CONSTEXPR inplace_merger inplace_merge {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
