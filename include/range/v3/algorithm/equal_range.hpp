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
#ifndef RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP
#define RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP

#include <utility>
#include <algorithm>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/is_infinity.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename Value,
                typename BinaryPredicate = ranges::less>
            iterator_range<ForwardIterator>
            equal_range(ForwardIterator begin, Sentinel end_, Value const & val,
                BinaryPredicate pred = BinaryPredicate{})
            {
                ForwardIterator end;
                auto dist = detail::distance(begin, end_, &end);
                RANGES_ASSERT(!ranges::is_infinity(dist));
                while(0 != dist)
                {
                    auto half = dist / 2;
                    auto middle = ranges::next(begin, half);
                    if(pred(*middle, val))
                    {
                        begin = std::move(++middle);
                        dist -= half + 1;
                    }
                    else if(pred(val, *middle))
                    {
                        end = std::move(middle);
                        dist = half;
                    }
                    else
                        return {
                            std::lower_bound(std::move(begin), middle, val, std::ref(pred)),
                            std::upper_bound(ranges::next(middle), end, val, std::ref(pred))
                        };
                }
                return {begin, begin};
            }
        }

        struct equal_ranger : bindable<equal_ranger>
        {
            /// \brief template function \c equal_ranger::operator()
            ///
            /// range-based version of the \c equal_range std algorithm
            ///
            /// \pre \c ForwardIterable is a model of the ForwardIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterable, typename Value,
                CONCEPT_REQUIRES(ranges::Iterable<ForwardIterable>() &&
                                 ranges::LessThanComparable<Value const &, range_reference_t<ForwardIterable>>() &&
                                 ranges::LessThanComparable<range_reference_t<ForwardIterable>, Value const &>())>
            static iterator_range<range_iterator_t<ForwardIterable>>
            invoke(equal_ranger, ForwardIterable && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                return detail::equal_range(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            template<typename ForwardIterable, typename Value, typename BinaryPredicate>
            static iterator_range<range_iterator_t<ForwardIterable>>
            invoke(equal_ranger, ForwardIterable && rng, Value const & val, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable>,
                                                       Value const &>());
                return detail::equal_range(ranges::begin(rng), ranges::end(rng), val,
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | equal_range(val)
            template<typename Value>
            static auto invoke(equal_ranger equal_range, Value && val)
                -> decltype(equal_range.move_bind(std::placeholders::_1, std::forward<Value>(val)))
            {
                return equal_range.move_bind(std::placeholders::_1, std::forward<Value>(val));
            }

            /// \overload
            /// for rng | equal_range(val, pred)
            template<typename Value, typename BinaryPredicate,
                CONCEPT_REQUIRES(
                    !(ranges::Iterable<Value>() &&
                      ranges::LessThanComparable<BinaryPredicate, range_reference_t<Value>>() &&
                      ranges::LessThanComparable<range_reference_t<Value>, BinaryPredicate>()))>
            static auto invoke(equal_ranger equal_range, Value && val, BinaryPredicate pred)
                -> decltype(equal_range.move_bind(std::placeholders::_1, std::forward<Value>(val),
                    std::move(pred)))
            {
                return equal_range.move_bind(std::placeholders::_1, std::forward<Value>(val),
                    std::move(pred));
            }
        };

        RANGES_CONSTEXPR equal_ranger equal_range {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
