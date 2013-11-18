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
#ifndef RANGES_V3_ALGORITHM_COUNT_IF_HPP
#define RANGES_V3_ALGORITHM_COUNT_IF_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct counter_if
        {
            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename UnaryPredicate, typename This = counter_if>
            auto operator()(UnaryPredicate pred) const
                -> decltype(bindable<This>{}(std::placeholders::_1, std::move(pred)))
            {
                return bindable<This>{}(std::placeholders::_1, std::move(pred));
            }

            /// \brief template function \c counter_if::operator()
            ///
            /// range-based version of the \c count_if std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename InputRange, typename UnaryPredicate>
            range_difference_t<InputRange>
            operator()(InputRange && rng, UnaryPredicate pred) const
            {
                static_assert(ranges::InputRange<InputRange>(),
                    "Expecting model of InputRange");
                return std::count_if(ranges::begin(rng), ranges::end(rng), std::move(pred));
            }
        };

        constexpr bindable<counter_if> count_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
