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
            struct InputRange
              : refines<CopyConstructible>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(ranges::begin(t), ranges::end(t)),
                        concepts::same_type(ranges::cbegin(t), ranges::cend(t)),
                        concepts::model_of<InputIterator>(ranges::begin(t)),
                        concepts::model_of<InputIterator>(ranges::cbegin(t)),
                        concepts::convertible_to<decltype(ranges::cbegin(t))>(ranges::begin(t))
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
    }
}

#endif
