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
#ifndef RANGES_V3_ALGORITHM_HEAP_ALGORITHM_HPP
#define RANGES_V3_ALGORITHM_HEAP_ALGORITHM_HPP

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
        struct heap_pusher : bindable<heap_pusher>, pipeable<heap_pusher>
        {
            /// \brief template function \c heap_pusher::operator()
            ///
            /// range-based version of the \c push_heap std algorithm
            ///
            /// \pre \c RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange
            invoke(heap_pusher, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::push_heap(ranges::begin(rng), ranges::end(rng));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange
            invoke(heap_pusher, RandomAccessRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::push_heap(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
                return detail::forward<RandomAccessRange>(rng);
            }

            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto
            invoke(heap_pusher push_heap, BinaryPredicate pred)
                -> decltype(push_heap(std::placeholders::_1, detail::move(pred)))
            {
                return push_heap(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR heap_pusher push_heap {};

        struct heap_popper : bindable<heap_popper>, pipeable<heap_popper>
        {
            /// \brief template function \c heap_popper::operator()
            ///
            /// range-based version of the \c pop_heap std algorithm
            ///
            /// \pre \c RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange
            invoke(heap_popper, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::pop_heap(ranges::begin(rng), ranges::end(rng));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange
            invoke(heap_popper, RandomAccessRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::pop_heap(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto
            invoke(heap_popper pop_heap, BinaryPredicate pred)
                -> decltype(pop_heap(std::placeholders::_1, detail::move(pred)))
            {
                return pop_heap(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR heap_popper pop_heap {};

        struct heap_maker : bindable<heap_maker>, pipeable<heap_maker>
        {
            /// \brief template function \c heap_maker::operator()
            ///
            /// range-based version of the \c make_heap std algorithm
            ///
            /// \pre \c RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange
            invoke(heap_maker, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::make_heap(ranges::begin(rng), ranges::end(rng));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange
            invoke(heap_maker, RandomAccessRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::make_heap(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto
            invoke(heap_maker make_heap, BinaryPredicate pred)
                -> decltype(make_heap(std::placeholders::_1, detail::move(pred)))
            {
                return make_heap(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR heap_maker make_heap {};

        struct heap_sorter : bindable<heap_sorter>, pipeable<heap_sorter>
        {
            /// \brief template function \c heap_sorter::operator()
            ///
            /// range-based version of the \c sort_heap std algorithm
            ///
            /// \pre \c RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename RandomAccessRange,
                CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange
            invoke(heap_sorter, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::sort_heap(ranges::begin(rng), ranges::end(rng));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename BinaryPredicate>
            static RandomAccessRange
            invoke(heap_sorter, RandomAccessRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::sort_heap(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
                return detail::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto
            invoke(heap_sorter sort_heap, BinaryPredicate pred)
                -> decltype(sort_heap(std::placeholders::_1, detail::move(pred)))
            {
                return sort_heap(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR heap_sorter sort_heap {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
