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

#ifndef RANGES_V3_CONCEPTS_HPP
#define RANGES_V3_CONCEPTS_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/begin_end.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct Range
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
                        concepts::same_type(ranges::begin(t), ranges::end(t)),
                        concepts::same_type(ranges::cbegin(t), ranges::cend(t)),
                        concepts::model_of<Iterator>(ranges::begin(t)),
                        concepts::model_of<Iterator>(ranges::cbegin(t)),
                        concepts::convertible_to<decltype(ranges::cbegin(t))>(ranges::begin(t))
                    ));
            };

            struct InputRange
              : refines<Range>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<InputIterator>(ranges::begin(t)),
                        concepts::model_of<InputIterator>(ranges::cbegin(t))
                    ));
            };

            struct ForwardRange
              : refines<InputRange>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ForwardIterator>(ranges::begin(t)),
                        concepts::model_of<ForwardIterator>(ranges::cbegin(t))
                    ));
            };

            struct BidirectionalRange
              : refines<ForwardRange>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BidirectionalIterator>(ranges::begin(t)),
                        concepts::model_of<BidirectionalIterator>(ranges::cbegin(t))
                    ));
            };

            struct RandomAccessRange
              : refines<BidirectionalRange>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<RandomAccessIterator>(ranges::begin(t)),
                        concepts::model_of<RandomAccessIterator>(ranges::cbegin(t))
                    ));
            };
        }

        template<typename T>
        constexpr bool Range()
        {
            return concepts::models<concepts::Range, T>();
        }

        template<typename T>
        constexpr bool InputRange()
        {
            return concepts::models<concepts::InputRange, T>();
        }

        template<typename T>
        constexpr bool ForwardRange()
        {
            return concepts::models<concepts::ForwardRange, T>();
        }

        template<typename T>
        constexpr bool BidirectionalRange()
        {
            return concepts::models<concepts::BidirectionalRange, T>();
        }

        template<typename T>
        constexpr bool RandomAccessRange()
        {
            return concepts::models<concepts::RandomAccessRange, T>();
        }

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
