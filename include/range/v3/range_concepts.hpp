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
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/begin_end.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct Iterable
              : refines<CopyConstructible>
            {
                // Associated types
                template<typename T>
                using iterator_t = decltype(ranges::begin(std::declval<T>()));

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

                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>(ranges::begin(t)),
                        concepts::model_of<Iterator>(ranges::cbegin(t)),
                        concepts::convertible_to<decltype(ranges::cbegin(t))>(ranges::begin(t)),
                        ranges::begin(t) == ranges::end(t),
                        ranges::cbegin(t) == ranges::cend(t)
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
        }

        template<typename T>
        using Iterable = concepts::models<concepts::Iterable, T>;

        template<typename T>
        using InputIterable = concepts::models<concepts::InputIterable, T>;

        template<typename T>
        using ForwardIterable = concepts::models<concepts::ForwardIterable, T>;

        template<typename T>
        using BidirectionalIterable = concepts::models<concepts::BidirectionalIterable, T>;

        template<typename T>
        using RandomAccessIterable = concepts::models<concepts::RandomAccessIterable, T>;

        template<typename T>
        using Range = concepts::models<concepts::Range, T>;

        template<typename T>
        using InputRange = concepts::models<concepts::InputRange, T>;

        template<typename T>
        using ForwardRange = concepts::models<concepts::ForwardRange, T>;

        template<typename T>
        using BidirectionalRange = concepts::models<concepts::BidirectionalRange, T>;

        template<typename T>
        using RandomAccessRange = concepts::models<concepts::RandomAccessRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // range_concept
        template<typename T>
        using range_concept_t =
            concepts::most_refined_t<concepts::RandomAccessRange, T>;

        template<typename T>
        struct range_concept
        {
            using type = range_concept_t<T>;
        };
    }
}

#endif
