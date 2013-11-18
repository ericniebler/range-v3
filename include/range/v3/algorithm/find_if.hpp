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
#ifndef RANGES_V3_ALGORITHM_FIND_IF_HPP
#define RANGES_V3_ALGORITHM_FIND_IF_HPP

#include <utility>
#include <algorithm>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct finder_if
        {
            /// \brief template function \c finder_if::operator()
            ///
            /// range-based version of the \c find_if std algorithm
            ///
            /// \pre \c InputRange is a model of the InputRange concept
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename InputRange, typename UnaryPredicate>
            range_iterator_t<InputRange> operator()(InputRange && rng, UnaryPredicate pred) const
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::find_if(ranges::begin(rng), ranges::end(rng), std::move(pred));
            }

            /// \overload
            template<typename UnaryPredicate, typename This = finder_if>
            auto operator()(UnaryPredicate pred) const
                -> decltype(bindable<This>{}(std::placeholders::_1, std::move(pred)))
            {
                return bindable<This>{}(std::placeholders::_1, std::move(pred));
            }
        };

        constexpr bindable<finder_if> find_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
