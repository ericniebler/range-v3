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
#ifndef RANGES_V3_ALGORITHM_MISMATCH_HPP
#define RANGES_V3_ALGORITHM_MISMATCH_HPP

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
        namespace detail
        {
            template<typename InputIterator1, typename InputIterator2>
            std::pair<InputIterator1, InputIterator2>
            mismatch_impl(InputIterator1 first1, InputIterator1 last1,
                          InputIterator2 first2, InputIterator2 last2)
            {
                while(first1 != last1 && first2 != last2 && *first1 == *first2)
                {
                    ++first1;
                    ++first2;
                }
                return {first1, first2};
            }

            template<typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
            std::pair<InputIterator1, InputIterator2>
            mismatch_impl(InputIterator1 first1, InputIterator1 last1,
                          InputIterator2 first2, InputIterator2 last2,
                          BinaryPredicate pred)
            {
                while(first1 != last1 && first2 != last2 && pred(*first1, *first2))
                {
                    ++first1;
                    ++first2;
                }
                return {first1, first2};
            }
        } // namespace range_detail

        struct mismatcher : bindable<mismatcher>
        {
            /// \brief template function mismatch
            ///
            /// range-based version of the mismatch std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2>
            static std::pair<range_iterator_t<InputRange1>, range_iterator_t<InputRange2>>
            invoke(mismatcher, InputRange1 && rng1, InputRange2 && rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<InputRange1>,
                                                          range_reference_t<InputRange2>> ());
                return ranges::detail::mismatch_impl(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename BinaryPredicate>
            static std::pair<range_iterator_t<InputRange1>, range_iterator_t<InputRange2>>
            invoke(mismatcher, InputRange1 && rng1, InputRange2 && rng2, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputRange1>,
                                                       range_reference_t<InputRange2>>());
                return ranges::detail::mismatch_impl(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR mismatcher mismatch {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
