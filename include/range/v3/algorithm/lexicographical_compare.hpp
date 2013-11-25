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
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct lexicographical_comparer : bindable<lexicographical_comparer>
        {
            /// \brief template function \s lexicographical_comparer::operator()
            ///
            /// range-based version of the \c lexicographical_compare std algorithm
            ///
            /// \pre \c InputRange1 is a model of the InputRange concept
            /// \pre \c InputRange2 is a model of the InputRange concept
            template<typename InputRange1, typename InputRange2>
            static bool invoke(lexicographical_comparer,
                               InputRange1 const & rng1,
                               InputRange2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<const InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<const InputRange2>());
                return std::lexicographical_compare(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2,
                     typename BinaryPredicate>
            static bool invoke(lexicographical_comparer,
                               InputRange1 const & rng1,
                               InputRange2 const & rng2,
                               BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<const InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<const InputRange2>());
                return std::lexicographical_compare(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(pred));
            }
        };

        constexpr lexicographical_comparer lexicographical_compare {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
