//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0.(See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_PARTITION_HPP
#define RANGES_V3_ALGORITHM_PARTITION_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
            ForwardIterator
            partition(ForwardIterator begin, Sentinel end, UnaryPredicate pred)
            {
                while(true)
                {
                    if(begin == end)
                        return begin;
                    if(!pred(*begin))
                        break;
                    ++begin;
                }
                for(auto tmp = begin; ++tmp != end;)
                {
                    if(pred(*tmp))
                    {
                        ranges::swap(*begin, *tmp);
                        ++begin;
                    }
                }
                return begin;
            }

            template<typename BidirectionalIterator, typename UnaryPredicate,
                CONCEPT_REQUIRES_(ranges::BidirectionalIterator<BidirectionalIterator>())>
            BidirectionalIterator
            partition(BidirectionalIterator begin, BidirectionalIterator end, UnaryPredicate pred)
            {
                while(true)
                {
                    while(true)
                    {
                        if(begin == end)
                            return begin;
                        if(!pred(*begin))
                            break;
                        ++begin;
                    }
                    do
                    {
                        if(begin == --end)
                            return begin;
                    } while(!pred(*end));
                    ranges::swap(*begin, *end);
                    ++begin;
                }
            }
        }

        struct partitioner : bindable<partitioner>
        {
            /// \brief template function partition
            ///
            /// range-based version of the partition std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename UnaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(partitioner, ForwardIterable && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                return detail::partition(ranges::begin(rng), ranges::end(rng),
                                         ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            template<typename UnaryPredicate>
            static auto invoke(partitioner partition, UnaryPredicate pred) ->
                decltype(partition.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return partition.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR partitioner partition {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
