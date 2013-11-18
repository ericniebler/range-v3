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
        struct counter
        {
            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            template<typename Value, typename This = counter>
            auto operator()(Value const & val) const
                -> decltype(bindable<This>{}(std::placeholders::_1, val))
            {
                return bindable<This>{}(std::placeholders::_1, val);
            }

            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            template<typename InputRange, typename Value>
            range_difference_t<InputRange>
            operator()(InputRange && rng, Value const & val) const
            {
                static_assert(ranges::InputRange<InputRange>(),
                    "Expecting model of InputRange");
                return std::count(ranges::begin(rng), ranges::end(rng), val);
            }
        };

        constexpr bindable<counter> count {};
    } // inline namespace v3

} // namespace ranges

#endif // include guard
