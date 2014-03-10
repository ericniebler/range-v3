// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_RANGE_CONCEPTS_HPP
#define RANGES_V3_RANGE_CONCEPTS_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Iterable, typename Void = void>
        struct is_infinite
          : std::is_base_of<detail::is_infinite<true>, Iterable>
        {};

        namespace concepts
        {
            struct Iterable
              : refines<CopyConstructible>
            {
                // Associated types
                template<typename T>
                using iterator_t = decltype(ranges::begin(std::declval<T>()));

                template<typename T>
                using sentinel_t = decltype(ranges::end(std::declval<T>()));

                template<typename T>
                using category_t = Iterator::category_t<iterator_t<T>>;

                template<typename T>
                using value_t = Iterator::value_t<iterator_t<T>>;

                template<typename T>
                using difference_t = Iterator::difference_t<iterator_t<T>>;

                template<typename T>
                using reference_t = Iterator::reference_t<iterator_t<T>>;

                template<typename T>
                using pointer_t = Iterator::pointer_t<iterator_t<T>>;

                template<typename T>
                using is_finite_t = detail::not_t<is_infinite<T>>;

                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>(ranges::begin(t)),
                        concepts::model_of<Iterator>(ranges::cbegin(t)),
                        concepts::convertible_to<decltype(ranges::cbegin(t))>(ranges::begin(t)),
                        concepts::convertible_to<decltype(ranges::cend(t))>(ranges::end(t)),
                        concepts::model_of<EqualityComparable>(ranges::begin(t), ranges::end(t)),
                        concepts::model_of<EqualityComparable>(ranges::cbegin(t), ranges::cend(t))
                    ));
            };

            struct OutputIterable
              : refines<Iterable(_1)>
            {
                template<typename T, typename O>
                auto requires(T && t, O && o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<OutputIterator>(ranges::begin(t), (O &&)o),
                        concepts::model_of<OutputIterator>(ranges::cbegin(t), (O &&)o)
                    ));
            };

            struct InputIterable
              : refines<Iterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<InputIterator>(ranges::begin(t)),
                        concepts::model_of<InputIterator>(ranges::cbegin(t))
                    ));
            };

            struct ForwardIterable
              : refines<InputIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ForwardIterator>(ranges::begin(t)),
                        concepts::model_of<ForwardIterator>(ranges::cbegin(t))
                    ));
            };

            struct BidirectionalIterable
              : refines<ForwardIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BidirectionalIterator>(ranges::begin(t)),
                        concepts::model_of<BidirectionalIterator>(ranges::cbegin(t))
                    ));
            };

            struct RandomAccessIterable
              : refines<BidirectionalIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<RandomAccessIterator>(ranges::begin(t)),
                        concepts::model_of<RandomAccessIterator>(ranges::cbegin(t))
                    ));
            };

            struct FiniteIterable
              : refines<Iterable>
            {
                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(Iterable::is_finite_t<T>{})
                    ));
            };

            struct FiniteOutputIterable
              : refines<FiniteIterable(_1), OutputIterable>
            {};

            struct FiniteInputIterable
              : refines<FiniteIterable, InputIterable>
            {};

            struct FiniteForwardIterable
              : refines<FiniteInputIterable, ForwardIterable>
            {};

            struct FiniteBidirectionalIterable
              : refines<FiniteForwardIterable, BidirectionalIterable>
            {};

            struct FiniteRandomAccessIterable
              : refines<FiniteBidirectionalIterable, RandomAccessIterable>
            {};

            struct Range
              : refines<Iterable>
            {
                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(ranges::begin(t), ranges::end(t)),
                        concepts::same_type(ranges::cbegin(t), ranges::cend(t))
                    ));
            };

            struct OutputRange
              : refines<Range(_1), OutputIterable>
            {};

            struct InputRange
              : refines<Range, InputIterable>
            {};

            struct ForwardRange
              : refines<InputRange, ForwardIterable>
            {};

            struct BidirectionalRange
              : refines<ForwardRange, BidirectionalIterable>
            {};

            struct RandomAccessRange
              : refines<BidirectionalRange, RandomAccessIterable>
            {};

            struct FiniteRange
              : refines<Range, FiniteIterable>
            {};

            struct FiniteOutputRange
              : refines<FiniteRange(_1), OutputRange>
            {};

            struct FiniteInputRange
              : refines<FiniteRange, InputRange>
            {};

            struct FiniteForwardRange
              : refines<FiniteInputRange, ForwardRange>
            {};

            struct FiniteBidirectionalRange
              : refines<FiniteForwardRange, BidirectionalRange>
            {};

            struct FiniteRandomAccessRange
              : refines<FiniteBidirectionalRange, RandomAccessRange>
            {};

            struct CountedSentinel
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<SignedIntegral>(t.count())
                    ));
            };

            struct CountedIterable
              : refines<FiniteIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<CountedIterator>(ranges::begin(t)),
                        concepts::model_of<CountedIterator>(ranges::cbegin(t)),
                        concepts::model_of<CountedSentinel>(ranges::end(t)),
                        concepts::model_of<CountedSentinel>(ranges::cend(t))
                    ));
            };

            struct CountedInputIterable
              : refines<CountedIterable, FiniteInputIterable>
            {};

            struct CountedForwardIterable
              : refines<CountedInputIterable, FiniteForwardIterable>
            {};

            struct CountedBidirectionalIterable
              : refines<CountedForwardIterable, FiniteBidirectionalIterable>
            {};

            struct CountedRandomAccessIterable
              : refines<CountedBidirectionalIterable, FiniteRandomAccessIterable>
            {};

            struct CountedRange
              : refines<CountedIterable, FiniteRange>
            {};

            struct CountedInputRange
              : refines<CountedRange, FiniteInputRange>
            {};

            struct CountedForwardRange
              : refines<CountedInputRange, FiniteForwardRange>
            {};

            struct CountedBidirectionalRange
              : refines<CountedForwardRange, FiniteBidirectionalRange>
            {};

            struct CountedRandomAccessRange
              : refines<CountedBidirectionalRange, FiniteRandomAccessRange>
            {};
        }

        template<typename T>
        using Iterable = concepts::models<concepts::Iterable, T>;

        template<typename T, typename O>
        using OutputIterable = concepts::models<concepts::OutputIterable, T, O>;

        template<typename T>
        using InputIterable = concepts::models<concepts::InputIterable, T>;

        template<typename T>
        using ForwardIterable = concepts::models<concepts::ForwardIterable, T>;

        template<typename T>
        using BidirectionalIterable = concepts::models<concepts::BidirectionalIterable, T>;

        template<typename T>
        using RandomAccessIterable = concepts::models<concepts::RandomAccessIterable, T>;

        template<typename T>
        using FiniteIterable = concepts::models<concepts::FiniteIterable, T>;

        template<typename T, typename O>
        using FiniteOutputIterable = concepts::models<concepts::FiniteOutputIterable, T, O>;

        template<typename T>
        using FiniteInputIterable = concepts::models<concepts::FiniteInputIterable, T>;

        template<typename T>
        using FiniteForwardIterable = concepts::models<concepts::FiniteForwardIterable, T>;

        template<typename T>
        using FiniteBidirectionalIterable = concepts::models<concepts::FiniteBidirectionalIterable, T>;

        template<typename T>
        using FiniteRandomAccessIterable = concepts::models<concepts::FiniteRandomAccessIterable, T>;

        template<typename T>
        using CountedIterable = concepts::models<concepts::CountedIterable, T>;

        template<typename T>
        using CountedInputIterable = concepts::models<concepts::CountedInputIterable, T>;

        template<typename T>
        using CountedForwardIterable = concepts::models<concepts::CountedForwardIterable, T>;

        template<typename T>
        using CountedBidirectionalIterable = concepts::models<concepts::CountedBidirectionalIterable, T>;

        template<typename T>
        using CountedRandomAccessIterable = concepts::models<concepts::CountedRandomAccessIterable, T>;

        template<typename T>
        using Range = concepts::models<concepts::Range, T>;

        template<typename T, typename O>
        using OutputRange = concepts::models<concepts::OutputRange, T, O>;

        template<typename T>
        using InputRange = concepts::models<concepts::InputRange, T>;

        template<typename T>
        using ForwardRange = concepts::models<concepts::ForwardRange, T>;

        template<typename T>
        using BidirectionalRange = concepts::models<concepts::BidirectionalRange, T>;

        template<typename T>
        using RandomAccessRange = concepts::models<concepts::RandomAccessRange, T>;

        template<typename T>
        using FiniteRange = concepts::models<concepts::FiniteRange, T>;

        template<typename T, typename O>
        using FiniteOutputRange = concepts::models<concepts::FiniteOutputRange, T, O>;

        template<typename T>
        using FiniteInputRange = concepts::models<concepts::FiniteInputRange, T>;

        template<typename T>
        using FiniteForwardRange = concepts::models<concepts::FiniteForwardRange, T>;

        template<typename T>
        using FiniteBidirectionalRange = concepts::models<concepts::FiniteBidirectionalRange, T>;

        template<typename T>
        using FiniteRandomAccessRange = concepts::models<concepts::FiniteRandomAccessRange, T>;

        template<typename T>
        using CountedRange = concepts::models<concepts::CountedRange, T>;

        template<typename T>
        using CountedInputRange = concepts::models<concepts::CountedInputRange, T>;

        template<typename T>
        using CountedForwardRange = concepts::models<concepts::CountedForwardRange, T>;

        template<typename T>
        using CountedBidirectionalRange = concepts::models<concepts::CountedBidirectionalRange, T>;

        template<typename T>
        using CountedRandomAccessRange = concepts::models<concepts::CountedRandomAccessRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // range_concept
        template<typename T>
        using range_concept_t =
            concepts::most_refined_t<concepts::CountedRandomAccessRange, T>;

        template<typename T>
        struct range_concept
        {
            using type = range_concept_t<T>;
        };
    }
}

#endif
