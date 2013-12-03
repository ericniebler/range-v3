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
#ifndef RANGES_V3_ALGORITHM_UPPER_BOUND_HPP
#define RANGES_V3_ALGORITHM_UPPER_BOUND_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct upper_bound_finder : bindable<upper_bound_finder>
        {
            /// \brief template function upper_bound
            ///
            /// range-based version of the upper_bound std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename Value>
            static range_iterator_t<ForwardRange>
            invoke(upper_bound_finder, ForwardRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::upper_bound(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            template<typename ForwardRange, typename Value, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(upper_bound_finder, ForwardRange && rng, Value const & val, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<BinaryPredicate,
                                                       Value const &,
                                                       range_reference_t<ForwardRange>>());
                return std::upper_bound(ranges::begin(rng), ranges::end(rng), val, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR upper_bound_finder upper_bound {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
