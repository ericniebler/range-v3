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
#ifndef RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator1, typename Sentinel1,
                     typename InputIterator2, typename Sentinel2,
                     typename BinaryPredicate = ranges::less>
            bool
            lexicographical_compare(InputIterator1 begin1, Sentinel1 end1,
                                    InputIterator2 begin2, Sentinel2 end2,
                                    BinaryPredicate pred = BinaryPredicate{})
            {
                for(; begin2 != end2; ++begin1, ++begin2)
                {
                    if(begin1 == end1 || pred(*begin1, *begin2))
                        return true;
                    if(pred(*begin2, *begin1))
                        return false;
                }
                return false;
            }
        }

        struct lexicographical_comparer : bindable<lexicographical_comparer>
        {
            /// \brief template function \s lexicographical_comparer::operator()
            ///
            /// range-based version of the \c lexicographical_compare std algorithm
            ///
            /// \pre \c InputIterable1 is a model of the InputIterable concept
            /// \pre \c InputIterable2 is a model of the InputIterable concept
            template<typename InputIterable1, typename InputIterable2>
            static bool invoke(lexicographical_comparer, InputIterable1 const & rng1,
                InputIterable2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<InputIterable1 const>,
                                                          range_reference_t<InputIterable2 const>>());
                return detail::lexicographical_compare(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputIterable1, typename InputIterable2, typename BinaryPredicate>
            static bool invoke(lexicographical_comparer, InputIterable1 const & rng1,
                InputIterable2 const & rng2, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::Predicate<invokable_t<BinaryPredicate>,
                                                 range_reference_t<InputIterable1 const>,
                                                 range_reference_t<InputIterable2 const>>());
                return detail::lexicographical_compare(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR lexicographical_comparer lexicographical_compare {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
