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
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct remover_copier : bindable<remover_copier>
        {
            /// \brief template function remove_copy
            ///
            /// range-based version of the remove_copy std algorithm
            ///
            /// \pre InputRange is a model of the InputRange concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre Value is a model of the EqualityComparable concept
            /// \pre Objects of type Value can be compared for equality with objects of
            /// InputIterator's value type.
            template<typename InputRange, typename OutputIterator, typename Value>
            static OutputIterator
            invoke(remover_copier, InputRange && rng, OutputIterator out, Value const & val)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputRange>>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<InputRange>,
                                                          Value const &>());
                return std::remove_copy(ranges::begin(rng), ranges::end(rng),
                    std::move(out), val);
            }

            /// \overload
            /// for rng | remove_copy(out, val)
            template<typename OutputIterator, typename Value>
            static auto
            invoke(remover_copier remove_copy, OutputIterator out, Value && val) ->
                decltype(remove_copy(std::placeholders::_1, std::move(out),
                    std::forward<Value>(val)))
            {
                return remove_copy(std::placeholders::_1, std::move(out),
                    std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR remover_copier remove_copy {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
