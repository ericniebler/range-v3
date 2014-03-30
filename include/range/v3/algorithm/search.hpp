//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_SEARCH_HPP
#define RANGES_V3_ALGORITHM_SEARCH_HPP

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
            // Credit to Howard Hinnant
            template<typename ForwardIterator1, typename Sentinel1,
                     typename ForwardIterator2, typename Sentinel2,
                     typename BinaryPredicate>
            ForwardIterator1
            search(ForwardIterator1 begin1, Sentinel1 end1,
                   ForwardIterator2 begin2, Sentinel2 end2, BinaryPredicate pred)
            {
                if(begin2 == end2)
                    return begin1;
                while(true)
                {
                    while(true)
                    {
                        if(begin1 == end1)
                            return begin1;
                        if(pred(*begin1, *begin2))
                            break;
                        ++begin1;
                    }
                    auto m1 = begin1;
                    auto m2 = begin2;
                    while(true)
                    {
                        if(++m2 == end2)
                            return begin1;
                        if(++m1 == end1)
                            return m1;
                        if(!pred(*m1, *m2))
                        {
                            ++begin1;
                            break;
                        }
                    }
                }
            }

            template<typename RandomAccessIterator1,
                     typename RandomAccessIterator2,
                     typename BinaryPredicate,
                     CONCEPT_REQUIRES_(ranges::RandomAccessIterator<RandomAccessIterator1>() &&
                                       ranges::RandomAccessIterator<RandomAccessIterator2>())>
            RandomAccessIterator1
            search(RandomAccessIterator1 begin1, RandomAccessIterator1 end1,
                   RandomAccessIterator2 begin2, RandomAccessIterator2 end2, BinaryPredicate pred)
            {
                return std::search(std::move(begin1), std::move(end1),
                    std::move(begin2), std::move(end2), std::ref(pred));
            }
        }

        struct searcher : bindable<searcher>
        {
            /// \brief template function search
            ///
            /// range-based version of the search std algorithm
            ///
            /// \pre ForwardIterable1 is a model of the ForwardIterable concept
            /// \pre ForwardIterable2 is a model of the ForwardIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterable1, typename ForwardIterable2,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable1>() &&
                                  ranges::Iterable<ForwardIterable2>())>
            static range_iterator_t<ForwardIterable1>
            invoke(searcher, ForwardIterable1 && rng1, ForwardIterable2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable1>>());
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable2 const>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable2 const>>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<ForwardIterable1>,
                                                          range_reference_t<ForwardIterable2 const>>());
                return detail::search(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), ranges::equal_to{});
            }

            /// \overload
            template<typename ForwardIterable1, typename ForwardIterable2, typename BinaryPredicate>
            static range_iterator_t<ForwardIterable1>
            invoke(searcher, ForwardIterable1 && rng1, ForwardIterable2 const & rng2,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable1>>());
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable2 const>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable2 const>>());
                CONCEPT_ASSERT(ranges::InvokablePredicate<BinaryPredicate,
                                                          range_reference_t<ForwardIterable1>,
                                                          range_reference_t<ForwardIterable2 const>>());
                return detail::search(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), ranges::make_invokable(std::move(pred)));
            }

            template<typename ForwardIterable2>
            static auto
            invoke(searcher search, ForwardIterable2 && rng2) ->
                decltype(search.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2)))
            {
                return search.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2));
            }

            /// \overload
            template<typename ForwardIterable2, typename BinaryPredicate,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable2>() &&
                                !ranges::Iterable<BinaryPredicate>())>
            static auto
            invoke(searcher search, ForwardIterable2 && rng2, BinaryPredicate pred) ->
                decltype(search.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2),
                    std::move(pred)))
            {
                return search.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2),
                    std::move(pred));
            }
        };

        RANGES_CONSTEXPR searcher search{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
