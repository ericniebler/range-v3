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
#ifndef RANGES_V3_ALGORITHM_FIND_HPP
#define RANGES_V3_ALGORITHM_FIND_HPP

#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct finder : bindable<finder>
        {
            /// \brief template function \c finder::operator()
            ///
            /// range-based version of the \c find std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            template<typename InputRange, typename Value>
            static range_iterator_t<InputRange>
            invoke(finder, InputRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::find(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            /// rng | find(val)
            template<typename Value>
            static auto invoke(finder find, Value && val)
                -> decltype(find(std::placeholders::_1, std::forward<Value>(val)))
            {
                return find(std::placeholders::_1, std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR finder find {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
