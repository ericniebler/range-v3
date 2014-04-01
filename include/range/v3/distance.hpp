// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_DISTANCE_HPP
#define RANGES_V3_DISTANCE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct count_fn
        {
        private:
            template<typename InputIterator, typename Sentinel, typename Distance>
            static std::pair<Distance, InputIterator>
            impl(InputIterator begin, Sentinel end, Distance d,
                concepts::InputIterator)
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }
            template<typename RandomAccessIterator, typename Distance>
            static std::pair<Distance, RandomAccessIterator>
            impl(RandomAccessIterator begin, RandomAccessIterator end, Distance d,
                concepts::RandomAccessIterator)
            {
                return {(end - begin) + d, end};
            }
        public:
            template<typename InputIterator, typename Sentinel,
                typename Distance = iterator_difference_t<InputIterator>,
                CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>() &&
                    ranges::Sentinel<Sentinel, InputIterator>() &&
                    ranges::Integral<Distance>())>
            std::pair<Distance, InputIterator>
            operator()(InputIterator begin, Sentinel end, Distance d = 0) const
            {
                return count_fn::impl(std::move(begin), std::move(end), d,
                    iterator_concept_t<InputIterator>{});
            }
            template<typename FiniteIterable,
                typename Distance = range_difference_t<FiniteIterable>,
                CONCEPT_REQUIRES_(ranges::Integral<Distance>() &&
                    ranges::Iterable<FiniteIterable>())>
            std::pair<Distance, range_iterator_t<FiniteIterable>>
            operator()(FiniteIterable &&rng, Distance d = 0) const
            {
                static_assert(!ranges::is_infinite<FiniteIterable>::value,
                    "Trying to compute the length of an infinite range!");
                return (*this)(ranges::begin(rng), ranges::end(rng), d);
            }
        };

        RANGES_CONSTEXPR count_fn count {};

        struct distance_fn
        {
        private:
            // Handle SizedIterables
            template<typename Iterable, typename Distance>
            static Distance
            impl(Iterable &rng, Distance d, concepts::Iterable)
            {
                return ranges::count(rng, d).first;
            }
            template<typename Iterable, typename Distance>
            static Distance
            impl(Iterable &rng, Distance d, concepts::SizedIterable)
            {
                return static_cast<Distance>(ranges::size(rng)) + d;
            }
        public:
            template<typename InputIterator, typename Sentinel,
                typename Distance = iterator_difference_t<InputIterator>,
                CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>() &&
                    ranges::Sentinel<Sentinel, InputIterator>() &&
                    ranges::Integral<Distance>())>
            Distance
            operator()(InputIterator begin, Sentinel end, Distance d = 0) const
            {
                return ranges::count(std::move(begin), std::move(end), d).first;
            }
            template<typename FiniteIterable,
                typename Distance = range_difference_t<FiniteIterable>,
                CONCEPT_REQUIRES_(ranges::Integral<Distance>() &&
                    ranges::Iterable<FiniteIterable>())>
            Distance
            operator()(FiniteIterable &&rng, Distance d = 0) const
            {
                static_assert(!ranges::is_infinite<FiniteIterable>::value,
                    "Trying to compute the length of an infinite range!");
                return distance_fn::impl(rng, d, range_concept_t<FiniteIterable>{});
            }
        };

        RANGES_CONSTEXPR distance_fn distance {};
    }
}

#endif
