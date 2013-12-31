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
#ifndef RANGES_V3_ALGORITHM_COPY_HPP
#define RANGES_V3_ALGORITHM_COPY_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copier : bindable<copier>
        {
            /// \brief template function copier::operator()
            ///
            /// range-based version of the \c copy std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            /// \pre \c OutputIterator is a model of the OutputIterator concept
            template<typename InputRange, typename OutputIterator>
            static OutputIterator invoke(copier, InputRange && rng, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputRange>>());
                return std::copy(ranges::begin(rng), ranges::end(rng), std::move(out));
            }

            /// \overload
            /// for rng | copy(out)
            template<typename OutputIterator>
            static auto invoke(copier copy, OutputIterator out)
                -> decltype(copy.move_bind(std::placeholders::_1, std::move(out)))
            {
                CONCEPT_ASSERT(ranges::Iterator<OutputIterator>());
                return copy.move_bind(std::placeholders::_1, std::move(out));
            }
        };

        RANGES_CONSTEXPR copier copy {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
