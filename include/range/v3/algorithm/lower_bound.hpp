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
#include <iterator>
#include <range/v3/distance.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename Value,
                typename BinaryPredicate = ranges::less>
            ForwardIterator
            lower_bound(ForwardIterator begin, Sentinel end, Value const& value,
                BinaryPredicate pred = BinaryPredicate{})
            {
                auto dist = detail::distance(begin, end).first;
                while(0 != dist)
                {
                    auto half = dist / 2;
                    auto middle = ranges::next(begin, half);
                    if(pred(*middle, value))
                    {
                        begin = std::move(++middle);
                        dist -= half + 1;
                    }
                    else
                        dist = half;
                }
                return begin;
            }
        }

        struct lower_bound_finder : bindable<lower_bound_finder>
        {
            /// \brief template function \c lower_bound_finder::operator()
            ///
            /// range-based version of the \c lower_bound std algorithm
            ///
            /// \pre \c ForwardIterable is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename Value,
                typename BinaryPredicate = ranges::less>
            static range_iterator_t<ForwardIterable>
            invoke(lower_bound_finder, ForwardIterable && rng, Value const & value,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::FiniteForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable>,
                                                       Value const &>());
                return detail::lower_bound(ranges::begin(rng), ranges::end(rng), value,
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | lower_bound(value)
            template<typename Value>
            static auto
            invoke(lower_bound_finder lower_bound, Value && value) ->
                decltype(lower_bound.move_bind(std::placeholders::_1, std::forward<Value>(value)))
            {
                return lower_bound.move_bind(std::placeholders::_1, std::forward<Value>(value));
            }

            /// \overload
            /// for rng | lower_bound(value, pred)
            template<typename Value, typename BinaryPredicate,
                CONCEPT_REQUIRES_(!ranges::Iterable<Value>())>
            static auto
            invoke(lower_bound_finder lower_bound, Value && value, BinaryPredicate pred) ->
                decltype(lower_bound.move_bind(std::placeholders::_1, std::forward<Value>(value),
                    std::move(pred)))
            {
                return lower_bound.move_bind(std::placeholders::_1, std::forward<Value>(value),
                    std::move(pred));
            }
        };

        RANGES_CONSTEXPR lower_bound_finder lower_bound {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
