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
#ifndef RANGES_V3_ALGORITHM_SEARCH_N_HPP
#define RANGES_V3_ALGORITHM_SEARCH_N_HPP

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
        namespace detail
        {
            // Credit to Howard Hinnant
            template<typename ForwardIterator, typename Sentinel, typename Size,
                     typename Value, typename BinaryPredicate>
            ForwardIterator
            search_n(ForwardIterator begin, Sentinel end,
                     Size count, Value const &value, BinaryPredicate pred,
                     concepts::ForwardIterable)
            {
                if (count <= 0)
                    return begin;
                while(true)
                {
                    while(true)
                    {
                        if(begin == end)
                            return begin;
                        if(pred(*begin, value))
                            break;
                        ++begin;
                    }
                    auto tmp = begin;
                    Size n{0};
                    while(true)
                    {
                        if(++n == count)
                            return begin;
                        if(++tmp == end)
                            return tmp;
                        if(!pred(*tmp, value))
                        {
                            begin = tmp;
                            ++begin;
                            break;
                        }
                    }
                }
            }

            template<typename RandomAccessIterator, typename Size,
                     typename Value, typename BinaryPredicate>
            RandomAccessIterator
            search_n(RandomAccessIterator begin, RandomAccessIterator end,
                     Size count, Value const &value, BinaryPredicate pred,
                     concepts::RandomAccessRange)
            {
                return std::search_n(std::move(begin), std::move(end),
                    std::move(count), value, std::ref(pred));
            }
        }

        struct searcher_n : bindable<searcher_n>
        {
            /// \brief template function search
            ///
            /// range-based version of the search_n std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            /// \pre Integer is an integral type
            /// \pre Value is a model of the EqualityComparable concept
            /// \pre ForwardIterable's value type is a model of the EqualityComparable concept
            /// \pre Objects of ForwardIterable's value type can be compared for equality with
            ///      objects of type Value
            template<typename ForwardIterable, typename Integer, typename Value,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>())>
            static range_iterator_t<ForwardIterable>
            invoke(searcher_n, ForwardIterable && rng, Integer count, Value const & value)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::Integral<Integer>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<ForwardIterable>,
                                                          Value const &>());
                return detail::search_n(ranges::begin(rng), ranges::end(rng), count, value,
                    ranges::equal_to{}, range_concept_t<ForwardIterable>{});
            }

            /// \overload
            template<typename ForwardIterable, typename Integer, typename Value,
                typename BinaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(searcher_n, ForwardIterable && rng, Integer count, Value const & value,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::Integral<Integer>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable>,
                                                       Value const &>());
                return detail::search_n(ranges::begin(rng), ranges::end(rng), count, value,
                    ranges::make_invokable(std::move(pred)), range_concept_t<ForwardIterable>{});
            }

            /// \overload
            template<typename Integer, typename Value>
            static auto
            invoke(searcher_n search_n, Integer count, Value && value) ->
                decltype(search_n.move_bind(std::placeholders::_1, std::move(count),
                    std::forward<Value>(value)))
            {
                return search_n.move_bind(std::placeholders::_1, std::move(count),
                    std::forward<Value>(value));
            }

            /// \overload
            template<typename Integer, typename Value, typename BinaryPredicate,
                CONCEPT_REQUIRES_(ranges::Integral<Integer>())>
            static auto
            invoke(searcher_n search_n, Integer count, Value && value, BinaryPredicate pred) ->
                decltype(search_n.move_bind(std::placeholders::_1, std::move(count),
                    std::forward<Value>(value), std::move(pred)))
            {
                return search_n.move_bind(std::placeholders::_1, std::move(count),
                    std::forward<Value>(value), std::move(pred));
            }
        };

        RANGES_CONSTEXPR searcher_n search_n{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
