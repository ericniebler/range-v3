///////////////////////////////////////////////////////////////////////////////
/// \file algorithm.hpp
///   Contains range-based versions of the std algorithms
//
// Copyright 2004 Eric Niebler.
// Copyright 2006 Thorsten Ottosen.
// Copyright 2009 Neil Groves.
//
// Distributed under the Boost Software License, Version 1.0.(See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_NUMERIC_HPP
#define RANGES_V3_NUMERIC_HPP

#include <numeric>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct Addable
            {
                template<typename T, typename U>
                using result_t = decltype(std::declval<T>() + std::declval<U>());

                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t + t
                    ));

                template<typename T, typename U>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        t + u
                    ));
            };
        }

        template<typename T, typename U = T>
        using Addable = concepts::models<concepts::Addable, T, U>;

        struct accumulate_ : bindable<accumulate_>
        {
            template<typename InputRange, typename Value,
                CONCEPT_REQUIRES(ranges::Range<InputRange>())>
            static Value invoke(accumulate_, InputRange && rng, Value init)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                CONCEPT_ASSERT(ranges::Addable<Value, range_reference_t<InputRange>>());
                return std::accumulate(ranges::begin(rng), ranges::end(rng),
                    std::move(init));
            }

            template<typename InputRange, typename Value, typename BinaryOperation>
            static Value invoke(accumulate_, InputRange && rng, Value init, BinaryOperation op)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                CONCEPT_ASSERT(ranges::Callable<invokable_t<BinaryOperation>,
                                                Value,
                                                range_reference_t<InputRange>>());
                return std::accumulate(ranges::begin(rng), ranges::end(rng),
                    std::move(init), make_invokable(std::move(op)));
            }

            template<typename Value>
            static auto invoke(accumulate_ accumulate, Value init)
                -> decltype(accumulate(std::placeholders::_1, std::move(init)))
            {
                return accumulate(std::placeholders::_1, std::move(init));
            }

            template<typename Value, typename BinaryOperation,
                CONCEPT_REQUIRES(!ranges::Range<Value>())>
            static auto invoke(accumulate_ accumulate, Value init, BinaryOperation op)
                -> decltype(accumulate(std::placeholders::_1, std::move(init), std::move(op)))
            {
                return accumulate(std::placeholders::_1, std::move(init), std::move(op));
            }
        };

        RANGES_CONSTEXPR accumulate_ accumulate {};

        struct inner_product_ : bindable<inner_product_>
        {
            template<typename InputRange1, typename InputRange2, typename Value>
            static Value
            invoke(inner_product_, InputRange1 && rng1, InputRange2 && rng2, Value init)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                return std::inner_product(ranges::begin(rng1), ranges::end(rng1),
                                          ranges::begin(rng2), std::move(init));
            }

            template<typename InputRange1, typename InputRange2,
                     typename Value, typename BinaryOperation1, typename BinaryOperation2>
            static Value
            invoke(inner_product_, InputRange1 && rng1, InputRange2 && rng2, Value init,
                BinaryOperation1 op1, BinaryOperation2 op2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                return std::inner_product(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), std::move(init), std::move(op1), std::move(op2));
            }
        };

        RANGES_CONSTEXPR inner_product_ inner_product {};

        struct partial_sum_ : bindable<partial_sum_>
        {
            template<typename InputRange, typename OutputIterator,
                CONCEPT_REQUIRES(ranges::Range<InputRange>())>
            static OutputIterator
            invoke(partial_sum_, InputRange && rng, OutputIterator result)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::partial_sum(ranges::begin(rng), ranges::end(rng), std::move(result));
            }

            template<typename InputRange, typename OutputIterator, typename BinaryOperation>
            static OutputIterator
            invoke(partial_sum_, InputRange && rng, OutputIterator result, BinaryOperation op)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::partial_sum(ranges::begin(rng), ranges::end(rng),
                    std::move(result), std::move(op));
            }

            template<typename OutputIterator>
            static auto invoke(partial_sum_ partial_sum, OutputIterator result)
                -> decltype(partial_sum(std::placeholders::_1, std::move(result)))
            {
                return partial_sum(std::placeholders::_1, std::move(result));
            }

            template<typename OutputIterator, typename BinaryOperation,
                CONCEPT_REQUIRES(!ranges::Range<OutputIterator>())>
            static auto invoke(partial_sum_ partial_sum, OutputIterator result, BinaryOperation op)
                -> decltype(partial_sum(std::placeholders::_1, std::move(result), std::move(op)))
            {
                return partial_sum(std::placeholders::_1, std::move(result), std::move(op));
            }
        };

        RANGES_CONSTEXPR partial_sum_ partial_sum {};

        struct adjacent_difference_ : bindable<adjacent_difference_>
        {
            template<typename InputRange, typename OutputIterator>
            static OutputIterator
            invoke(adjacent_difference_, InputRange && rng, OutputIterator result)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::adjacent_difference(ranges::begin(rng), ranges::end(rng),
                                                 result);
            }

            template<typename InputRange, typename OutputIterator, typename BinaryOperation>
            static OutputIterator
            invoke(adjacent_difference_, InputRange && rng, OutputIterator result,
                BinaryOperation op)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                return std::adjacent_difference(ranges::begin(rng), ranges::end(rng),
                                                 result, op);
            }

            template<typename OutputIterator>
            static auto invoke(adjacent_difference_ adjacent_difference, OutputIterator result)
                -> decltype(adjacent_difference(std::placeholders::_1, std::move(result)))
            {
                return adjacent_difference(std::placeholders::_1, std::move(result));
            }

            template<typename OutputIterator, typename BinaryOperation,
                CONCEPT_REQUIRES(!ranges::Range<OutputIterator>())>
            static auto invoke(adjacent_difference_ adjacent_difference, OutputIterator result,
                BinaryOperation op)
                -> decltype(adjacent_difference(std::placeholders::_1, std::move(result),
                        std::move(op)))
            {
                return adjacent_difference(std::placeholders::_1, std::move(result), std::move(op));
            }
        };

        RANGES_CONSTEXPR adjacent_difference_ adjacent_difference {};

        struct iota_ : bindable<iota_>
        {
            template<typename ForwardRange, typename Value>
            static void invoke(iota_, ForwardRange && rng, Value value)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::iota(ranges::begin(rng), ranges::end(rng), std::move(value));
            }

            template<typename Value>
            static auto invoke(iota_ iota, Value value)
                -> decltype(iota(std::placeholders::_1, std::move(value)))
            {
                return iota(std::placeholders::_1, std::move(value));
            }
        };

        RANGES_CONSTEXPR iota_ iota {};
    }
}

#endif
