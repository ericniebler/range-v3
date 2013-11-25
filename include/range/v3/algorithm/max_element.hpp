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
#ifndef RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct max_element_finder : bindable<max_element_finder>,
                                    pipeable<max_element_finder>
        {
            /// \brief template function max_element
            ///
            /// range-based version of the max_element std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange>
            static range_iterator_t<ForwardRange>
            invoke(max_element_finder, ForwardRange && rng)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::max_element(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardRange, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(max_element_finder, ForwardRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::max_element(ranges::begin(rng), ranges::end(rng), detail::move(pred));
            }
        };

        constexpr max_element_finder max_element {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
