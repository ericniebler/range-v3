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
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_COPY_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_COPY_HPP

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
        // I've got a bad feeling about this. std::partial_sort_copy writes into
        // and output random-access range and returns an iterator. In the range world,
        // it's probably desirable to keep any temporary output range alive
        // and do further processing on it, like this:
        //
        //   istream<int>(std::cin) | partial_sort_copy(std::vector<int>(20,0)) | ...
        //
        // That at least is what I was thinking when I wrote this.
        //
        // May-haps it's better to require folks to pass an lvalue
        // output range and let people use range placeholder expressions to build a
        // range for additional processing, like:
        //
        //   std::vector<int> out(20,0);
        //   istream<int>(std::cin) | range(out.begin(), partial_sort_copy(out)) | ...
        //
        // Or maybe output ranges need a rethink. This algo is certainly an interesting
        // case study. It most definitely needs a fixed-size, random-access output range,
        // so an output "sink" is not good enough.
        template<typename Range>
        struct partial_sort_copy_result
        {
            using iterator = range_iterator_t<Range>;
            using const_iterator = range_iterator_t<Range const>;

            partial_sort_copy_result(Range && rng, iterator end)
              : rng_(std::forward<Range>(rng)), end_(std::move(end))
            {}
            iterator begin()
            {
                return ranges::begin(rng_);
            }
            const_iterator begin() const
            {
                return ranges::begin(rng_);
            }
            iterator end()
            {
                return end_;
            }
            const_iterator end() const
            {
                return end_;
            }
            iterator base_end()
            {
                return ranges::end(rng_);
            }
            const_iterator base_end() const
            {
                return ranges::end(rng_);
            }
            bool operator!() const
            {
                return ranges::begin(rng_) == end_;
            }
            explicit operator bool() const
            {
                return ranges::begin(rng_) != end_;
            }
            operator iterator() const
            {
                return end_;
            }
            Range & base()
            {
                return rng_;
            }
            Range const & base() const
            {
                return rng_;
            }
        private:
            Range rng_;
            iterator end_;
        };

        struct partial_sorter_copier : bindable<partial_sorter_copier>
        {
            /// \brief template function partial_sort_copy
            ///
            /// range-based version of the partial_sort_copy std algorithm
            ///
            /// \pre InputRange is a model of the InputRange concept
            /// \pre RandomAccessRange is a model of the Mutable_RandomAccessRangeConcept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange, typename RandomAccessRange,
                CONCEPT_REQUIRES_(ranges::Range<InputRange>() && ranges::Range<RandomAccessRange>())>
            static partial_sort_copy_result<RandomAccessRange>
            invoke(partial_sorter_copier, InputRange && rng1, RandomAccessRange && rng2)
            {
                CONCEPT_ASSERT(ranges::Range<InputRange>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputRange>>());
                CONCEPT_ASSERT(ranges::Range<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::RandomAccessIterator<range_iterator_t<RandomAccessRange>>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<InputRange>>());
                return {std::forward<RandomAccessRange>(rng2),
                        std::partial_sort_copy(ranges::begin(rng1), ranges::end(rng1),
                                               ranges::begin(rng2), ranges::end(rng2))};
            }

            /// \overload
            template<typename InputRange, typename RandomAccessRange, typename BinaryPredicate>
            static partial_sort_copy_result<RandomAccessRange>
            invoke(partial_sorter_copier, InputRange && rng1, RandomAccessRange && rng2,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Range<InputRange>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputRange>>());
                CONCEPT_ASSERT(ranges::Range<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::RandomAccessIterator<range_iterator_t<RandomAccessRange>>());
                CONCEPT_ASSERT(ranges::InvokablePredicate<BinaryPredicate,
                                                          range_reference_t<InputRange>,
                                                          range_reference_t<InputRange>>());
                return {std::forward<RandomAccessRange>(rng2),
                        std::partial_sort_copy(ranges::begin(rng1), ranges::end(rng1),
                                               ranges::begin(rng2), ranges::end(rng2),
                                               ranges::make_invokable(std::move(pred)))};
            }

            template<typename RandomAccessRange>
            static auto
            invoke(partial_sorter_copier partial_sort_copy, RandomAccessRange && rng2) ->
                decltype(partial_sort_copy.move_bind(std::placeholders::_1,
                                              std::forward<RandomAccessRange>(rng2)))
            {
                CONCEPT_ASSERT(ranges::Range<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::RandomAccessIterator<range_iterator_t<RandomAccessRange>>());
                return partial_sort_copy.move_bind(std::placeholders::_1,
                                         std::forward<RandomAccessRange>(rng2));
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate,
                CONCEPT_REQUIRES_(ranges::Range<RandomAccessRange>() &&
                                !ranges::Range<BinaryPredicate>())>
            static auto
            invoke(partial_sorter_copier partial_sort_copy, RandomAccessRange && rng2,
                   BinaryPredicate pred) ->
                decltype(partial_sort_copy.move_bind(std::placeholders::_1,
                                              std::forward<RandomAccessRange>(rng2),
                                              std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::Range<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::RandomAccessIterator<range_iterator_t<RandomAccessRange>>());
                return partial_sort_copy.move_bind(std::placeholders::_1,
                                         std::forward<RandomAccessRange>(rng2),
                                         std::move(pred));
            }
        };

        RANGES_CONSTEXPR partial_sorter_copier partial_sort_copy {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
