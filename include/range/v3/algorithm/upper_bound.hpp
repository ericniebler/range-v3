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
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

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
            template<typename ForwardRange, typename Value,
                CONCEPT_REQUIRES(ranges::Range<ForwardRange>() &&
                                 ranges::EqualityComparable<Value const &,
                                                            range_reference_t<ForwardRange>>())>
            static range_iterator_t<ForwardRange>
            invoke(upper_bound_finder, ForwardRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::FiniteForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::EqualityComparable<Value const &,
                                                          range_reference_t<ForwardRange>>());
                return std::upper_bound(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            template<typename ForwardRange, typename Value, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(upper_bound_finder, ForwardRange && rng, Value const & val, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::FiniteForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       Value const &,
                                                       range_reference_t<ForwardRange>>());
                return std::upper_bound(ranges::begin(rng), ranges::end(rng), val,
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | upper_bound(val)
            template<typename Value>
            static auto
            invoke(upper_bound_finder upper_bound, Value && val) ->
                decltype(upper_bound.move_bind(std::placeholders::_1, std::forward<Value>(val)))
            {
                return upper_bound.move_bind(std::placeholders::_1, std::forward<Value>(val));
            }

            /// \overload
            template<typename Value, typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<Value>() ||
                                 !ranges::EqualityComparable<BinaryPredicate,
                                                             range_reference_t<Value>>())>
            static auto
            invoke(upper_bound_finder upper_bound, Value && val, BinaryPredicate pred) ->
                decltype(upper_bound.move_bind(std::placeholders::_1, std::forward<Value>(val),
                    std::move(pred)))
            {
                return upper_bound.move_bind(std::placeholders::_1, std::forward<Value>(val),
                    std::move(pred));
            }
        };

        RANGES_CONSTEXPR upper_bound_finder upper_bound {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
