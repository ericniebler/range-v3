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
        struct counter_if : bindable<counter_if>
        {
            /// \brief template function \c counter_if::operator()
            ///
            /// range-based version of the \c count_if std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename InputRange, typename UnaryPredicate>
            static range_difference_t<InputRange>
            invoke(counter_if, InputRange && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::count_if(ranges::begin(rng), ranges::end(rng), detail::move(pred));
            }

            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename UnaryPredicate>
            static auto invoke(counter_if count_if, UnaryPredicate pred)
                -> decltype(count_if(std::placeholders::_1, detail::move(pred)))
            {
                return count_if(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR counter_if count_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
