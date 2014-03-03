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
#include <range/v3/next_prev.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel,
                     typename Value, typename BinaryPredicate>
            ForwardIterator
            upper_bound(ForwardIterator begin, ForwardIterator end,
                        Value const & value, BinaryPredicate pred)
            {
                auto dist = detail::distance(begin, end).first;
                while(dist != 0)
                {
                    auto half = dist / 2;
                    auto middle = ranges::next(begin, half);
                    if(pred(value, *middle))
                        dist = half;
                    else
                    {
                        begin = std::move(++middle);
                        dist -= half + 1;
                    }
                }
                return begin;
            }
        }

        struct upper_bound_finder : bindable<upper_bound_finder>
        {
            /// \brief template function upper_bound
            ///
            /// range-based version of the upper_bound std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename Value,
                typename BinaryPredicate = ranges::less,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>())>
            static range_iterator_t<ForwardIterable>
            invoke(upper_bound_finder, ForwardIterable && rng, Value const & value,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::FiniteForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       Value const &,
                                                       range_reference_t<ForwardIterable>>());
                return detail::upper_bound(
                    ranges::begin(rng), ranges::end(rng),
                    value, ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | upper_bound(value)
            template<typename Value, typename BinaryPredicate = ranges::less,
                CONCEPT_REQUIRES_(!ranges::Iterable<Value>())>
            static auto
            invoke(upper_bound_finder upper_bound, Value && value,
                BinaryPredicate pred = BinaryPredicate{}) ->
                decltype(upper_bound.move_bind(std::placeholders::_1, std::forward<Value>(value),
                    std::move(pred)))
            {
                return upper_bound.move_bind(std::placeholders::_1, std::forward<Value>(value),
                    std::move(pred));
            }
        };

        RANGES_CONSTEXPR upper_bound_finder upper_bound {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
