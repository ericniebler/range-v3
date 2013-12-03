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
#ifndef RANGES_V3_ALGORITHM_LOWER_BOUND_HPP
#define RANGES_V3_ALGORITHM_LOWER_BOUND_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct lower_bound_finder : bindable<lower_bound_finder>
        {
            /// \brief template function \c lower_bound_finder::operator()
            ///
            /// range-based version of the \c lower_bound std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename Value>
            static range_iterator_t<ForwardRange>
            invoke(lower_bound_finder, ForwardRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::lower_bound(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            template<typename ForwardRange, typename Value, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(lower_bound_finder, ForwardRange && rng, Value const & val, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::lower_bound(ranges::begin(rng), ranges::end(rng), val,
                    ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR lower_bound_finder lower_bound {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
