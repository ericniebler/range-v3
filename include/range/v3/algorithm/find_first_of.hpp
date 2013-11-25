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
#ifndef RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP
#define RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP

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
        struct first_of_finder : bindable<first_of_finder>
        {
            /// \brief template function \c first_of_finder::operator()
            ///
            /// range-based version of the \c find_first_of std algorithm
            ///
            /// \pre \c InputRange1 is a model of the InputRange concept
            /// \pre \c ForwardRange2 is a model of the ForwardRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename ForwardRange2>
            static range_iterator_t<InputRange1>
            invoke(first_of_finder, InputRange1 && rng1, ForwardRange2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<const ForwardRange2>());

                return std::find_first_of(ranges::begin(rng1), ranges::end(rng1),
                                          ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename ForwardRange2, typename BinaryPredicate>
            static range_iterator_t<InputRange1>
            invoke(InputRange1 && rng1, ForwardRange2 const & rng2, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<const ForwardRange2>());

                return std::find_first_of(ranges::begin(rng1), ranges::end(rng1),
                                          ranges::begin(rng2), ranges::end(rng2), detail::move(pred));
            }
        };

        constexpr first_of_finder find_first_of {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
