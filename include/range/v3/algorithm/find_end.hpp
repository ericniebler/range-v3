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
#ifndef RANGES_V3_ALGORITHM_FIND_END_HPP
#define RANGES_V3_ALGORITHM_FIND_END_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
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
            template<typename ForwardIterator1, typename Sentinel1,
                     typename ForwardIterator2, typename Sentinel2,
                     typename BinaryPredicate>
            ForwardIterator1
            find_end(ForwardIterator1 begin1, Sentinel1 end1,
                     ForwardIterator2 begin2, Sentinel2 end2, BinaryPredicate pred,
                     concepts::ForwardIterable, concepts::ForwardIterable)
            {
                bool found = false;
                ForwardIterator1 res;
                while(true)
                {
                    while(true)
                    {
                        if(begin1 == end1)
                            return found ? res : begin1;
                        if(pred(*begin1, *begin2))
                            break;
                        ++begin1;
                    }
                    auto tmp1 = begin1;
                    auto tmp2 = begin2;
                    while(true)
                    {
                        if(++tmp2 == end2)
                        {
                            res = begin1++;
                            found = true;
                            break;
                        }
                        if(++tmp1 == end1)
                            return found ? res : tmp1;
                        if(!pred(*tmp1, *tmp2))
                        {
                            ++begin1;
                            break;
                        }
                    }
                }
            }

            // BUGBUG For Bidirectional and RandomAccesss Iterables, it's probably better to find
            // the end and dispatch to std::find_end.

            template<typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
            ForwardIterator1
            find_end(ForwardIterator1 begin1, ForwardIterator1 end1,
                     ForwardIterator2 begin2, ForwardIterator2 end2, BinaryPredicate pred,
                     concepts::ForwardRange, concepts::ForwardRange)
            {
                return std::find_end(std::move(begin1), std::move(end1),
                                     std::move(begin2), std::move(end2),
                                     std::move(pred));
            }
        }

        struct end_finder : bindable<end_finder>
        {
            /// \brief template function \c end_finder::operator()
            ///
            /// range-based version of the \c find_end std algorithm
            ///
            /// \pre \c ForwardIterable1 is a model of the ForwardIterable concept
            /// \pre \c ForwardIterable2 is a model of the ForwardIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterable1, typename ForwardIterable2,
                CONCEPT_REQUIRES(ranges::Iterable<ForwardIterable1>() &&
                                 ranges::Iterable<ForwardIterable2 const>())>
            static range_iterator_t<ForwardIterable1>
            invoke(end_finder, ForwardIterable1 && rng1, ForwardIterable2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2 const>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<ForwardIterable1>,
                                                          range_reference_t<ForwardIterable2 const>>());
                return detail::find_end(ranges::begin(rng1), ranges::end(rng1),
                                        ranges::begin(rng2), ranges::end(rng2),
                                        ranges::equal_to{},
                                        range_concept_t<ForwardIterable1>{},
                                        range_concept_t<ForwardIterable2>{});
            }

            /// \overload
            template<typename ForwardIterable1, typename ForwardIterable2, typename BinaryPredicate>
            static range_iterator_t<ForwardIterable1>
            invoke(end_finder, ForwardIterable1 && rng1, ForwardIterable2 const & rng2,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2 const>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable1>,
                                                       range_reference_t<ForwardIterable2 const>>());
                return detail::find_end(ranges::begin(rng1), ranges::end(rng1),
                                        ranges::begin(rng2), ranges::end(rng2),
                                        ranges::make_invokable(std::move(pred)),
                                        range_concept_t<ForwardIterable1>{},
                                        range_concept_t<ForwardIterable2>{});
            }

            /// \overload
            /// for rng | find_end(rng2)
            template<typename ForwardIterable2>
            static auto
            invoke(end_finder find_end, ForwardIterable2 && rng2) ->
                decltype(find_end.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2>());
                return find_end.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2));
            }

            /// \overload
            /// for rng | find_end(rng2, pred)
            template<typename ForwardIterable2, typename BinaryPredicate,
                CONCEPT_REQUIRES(ranges::Iterable<ForwardIterable2>() &&
                                !ranges::Iterable<BinaryPredicate>())>
            static auto
            invoke(end_finder find_end, ForwardIterable2 && rng2, BinaryPredicate pred) ->
                decltype(find_end.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2), std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2>());
                return find_end.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2), std::move(pred));
            }
        };

        RANGES_CONSTEXPR end_finder find_end {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
