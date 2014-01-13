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
                     typename BinaryPredicate = ranges::equal_to>
            bool equal(InputIterator1 begin1, Sentinel1 end1,
                       InputIterator2 begin2, Sentinel2 end2,
                       BinaryPredicate pred = BinaryPredicate{})
            {
                for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                    if(!pred(*begin1, *begin2))
                        return false;
                return begin1 == end1 && begin2 == end2;
            }
        }

        struct equaler : bindable<equaler>
        {
            /// \brief template function \c equaler::operator()
            ///
            /// range-based version of the \c equal std algorithm
            ///
            /// \pre \c InputIterable1 is a model of the InputIterable concept
            /// \pre \c InputIterable2 is a model of the InputIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2>
            static bool invoke(equaler, InputIterable1 && rng1, InputIterable2 && rng2)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable2>());
                return detail::equal(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputIterable1, typename InputIterable2, typename BinaryPredicate>
            static bool invoke(equaler, InputIterable1 && rng1, InputIterable2 && rng2,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable2>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputIterable1>,
                                                       range_reference_t<InputIterable2>>());
                return detail::equal(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2),
                                     ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR equaler equal {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
