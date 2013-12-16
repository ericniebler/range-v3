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
        struct equaler : bindable<equaler>
        {
            /// \brief template function \c equaler::operator()
            ///
            /// range-based version of the \c equal std algorithm
            ///
            /// \pre \c InputRange1 is a model of the InputRange concept
            /// \pre \c InputRange2 is a model of the InputRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2>
            static bool invoke(equaler, InputRange1 && rng1, InputRange2 && rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                return std::equal(ranges::begin(rng1), ranges::end(rng1),
                                  ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename BinaryPredicate>
            static bool invoke(equaler, InputRange1 && rng1, InputRange2 && rng2,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputRange1>,
                                                       range_reference_t<InputRange2>>());
                return std::equal(ranges::begin(rng1), ranges::end(rng1),
                                  ranges::begin(rng2), ranges::end(rng2),
                                  ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR equaler equal {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
