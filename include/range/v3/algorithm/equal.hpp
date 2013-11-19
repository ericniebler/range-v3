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
#ifndef RANGES_V3_ALGORITHM_EQUAL_HPP
#define RANGES_V3_ALGORITHM_EQUAL_HPP

#include <utility>
#include <iterator>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // An implementation of equality comparison that is optimized for iterator
            // traversal categories less than RandomAccess.
            template<typename InputIterator1,
                     typename InputIterator2>
            bool equal_impl(InputIterator1 first1,
                            InputIterator1 last1,
                            InputIterator2 first2,
                            InputIterator2 last2,
                            std::input_iterator_tag,
                            std::input_iterator_tag)
            {
                while(true)
                {
                    // If we have reached the end of the left range then this is
                    // the end of the loop. They are equal if and only if we have
                    // simultaneously reached the end of the right range.
                    if(first1 == last1)
                        return first2 == last2;

                    // If we have reached the end of the right range at this line
                    // it indicates that the right range is shorter than the left
                    // and hence the result is false.
                    if(first2 == last2)
                        return false;

                    // continue looping if and only if the values are equal
                    if(*first1 != *first2)
                        break;

                    ++first1;
                    ++first2;
                }

                // Reaching this line in the algorithm indicates that a value
                // inequality has been detected.
                return false;
            }

            template<typename InputIterator1,
                     typename InputIterator2,
                     typename BinaryPredicate>
            bool equal_impl(InputIterator1 first1,
                            InputIterator1 last1,
                            InputIterator2 first2,
                            InputIterator2 last2,
                            BinaryPredicate pred,
                            std::input_iterator_tag,
                            std::input_iterator_tag)
            {
                while(true)
                {
                    // If we have reached the end of the left range then this is
                    // the end of the loop. They are equal if and only if we have
                    // simultaneously reached the end of the right range.
                    if(first1 == last1)
                        return first2 == last2;

                    // If we have reached the end of the right range at this line
                    // it indicates that the right range is shorter than the left
                    // and hence the result is false.
                    if(first2 == last2)
                        return false;

                    // continue looping if and only if the values are equal
                    if(!pred(*first1, *first2))
                        break;

                    ++first1;
                    ++first2;
                }

                // Reaching this line in the algorithm indicates that a value
                // inequality has been detected.
                return false;
            }

            // An implementation of equality comparison that is optimized for
            // random access iterators.
            template<typename RandomAccessIterator1,
                     typename RandomAccessIterator2>
            bool equal_impl(RandomAccessIterator1 first1,
                            RandomAccessIterator1 last1,
                            RandomAccessIterator2 first2,
                            RandomAccessIterator2 last2,
                            std::random_access_iterator_tag,
                            std::random_access_iterator_tag)
            {
                return ((last1 - first1) == (last2 - first2))
                    && std::equal(std::move(first1), std::move(last1), std::move(first2));
            }

            template<typename RandomAccessIterator1,
                     typename RandomAccessIterator2,
                     typename BinaryPredicate>
            bool equal_impl(RandomAccessIterator1 first1,
                            RandomAccessIterator1 last1,
                            RandomAccessIterator2 first2,
                            RandomAccessIterator2 last2,
                            BinaryPredicate pred,
                            std::random_access_iterator_tag,
                            std::random_access_iterator_tag)
            {
                return ((last1 - first1) == (last2 - first2))
                    && std::equal(std::move(first1), std::move(last1),
                                  std::move(first2), std::move(pred));
            }

            template<typename InputIterator1,
                     typename InputIterator2>
            bool equal(InputIterator1 first1,
                       InputIterator1 last1,
                       InputIterator2 first2,
                       InputIterator2 last2)
            {
                typename std::iterator_traits< InputIterator1 >::iterator_category tag1;
                typename std::iterator_traits< InputIterator2 >::iterator_category tag2;

                return detail::equal_impl(
                    std::move(first1), std::move(last1),
                    std::move(first2), std::move(last2),
                    tag1, tag2);
            }

            template<typename InputIterator1,
                     typename InputIterator2,
                     typename BinaryPredicate>
            bool equal(InputIterator1 first1,
                       InputIterator1 last1,
                       InputIterator2 first2,
                       InputIterator2 last2,
                       BinaryPredicate pred)
            {
                typename std::iterator_traits< InputIterator1 >::iterator_category tag1;
                typename std::iterator_traits< InputIterator2 >::iterator_category tag2;

                return detail::equal_impl(
                    std::move(first1), std::move(last1),
                    std::move(first2), std::move(last2),
                    std::move(pred), tag1, tag2);
            }

        } // namespace detail

        struct equaler
        {
            /// \brief template function \c equaler::operator()
            ///
            /// range-based version of the \c equal std algorithm
            ///
            /// \pre \c InputRange1 is a model of the InputRange concept
            /// \pre \c InputRange2 is a model of the InputRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2>
            bool operator()(InputRange1 && rng1, InputRange2 && rng2) const
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());

                return ranges::detail::equal(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename BinaryPredicate>
            bool operator()(InputRange1 && rng1, InputRange2 && rng2,
                            BinaryPredicate pred) const
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());

                return ranges::detail::equal(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    std::move(pred));
            }
        };

        constexpr bindable<equaler> equal {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
