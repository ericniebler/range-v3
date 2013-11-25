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
#ifndef RANGES_V3_ALGORITHM_COUNT_HPP
#define RANGES_V3_ALGORITHM_COUNT_HPP

#include <algorithm>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct counter : bindable<counter>
        {
            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            template<typename InputRange, typename Value>
            static range_difference_t<InputRange>
            invoke(counter, InputRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::count(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            template<typename Value>
            static auto invoke(counter count, Value const & val)
                -> decltype(count(std::placeholders::_1, val))
            {
                return count(std::placeholders::_1, val);
            }
        };

        constexpr counter count {};
    } // inline namespace v3

} // namespace ranges

#endif // include guard
