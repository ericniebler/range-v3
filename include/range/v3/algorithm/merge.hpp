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
#include <range/v3/range_concepts.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator1, typename Sentinel1,
                     typename InputIterator2, typename Sentinel2,
                     typename OutputIterator,
                     typename BinaryPredicate = ranges::less>
            OutputIterator
            merge(InputIterator1 begin1, Sentinel1 end1,
                  InputIterator2 begin2, Sentinel2 end2,
                  OutputIterator out,
                  BinaryPredicate pred = BinaryPredicate{})
            {
                for (; begin1 != end1; ++out)
                {
                    if(begin2 == end2)
                        return detail::copy(std::move(begin1), std::move(end1), std::move(out));
                    *out = pred(*begin2, *begin1) ? *begin2++ : *begin1++;
                }
                return detail::copy(std::move(begin2), std::move(end2), std::move(out));
            }
        }

        struct merger : bindable<merger>
        {
            /// \brief template function merge
            ///
            /// range-based version of the merge std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator>
            static OutputIterator
            invoke(merger, InputIterable1 && rng1, InputIterable2 && rng2, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2>>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<InputIterable1>,
                                                          range_reference_t<InputIterable2>> ());
                return detail::merge(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2), std::move(out));
            }

            /// \overload
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator,
                     typename BinaryPredicate>
            static OutputIterator
            invoke(merger, InputIterable1 && rng1, InputIterable2 && rng2, OutputIterator out,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2>>());
                CONCEPT_ASSERT(ranges::InvokablePredicate<BinaryPredicate,
                                                          range_reference_t<InputIterable1>,
                                                          range_reference_t<InputIterable2>>());
                return detail::merge(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2),
                                     std::move(out),
                                     ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR merger merge {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
