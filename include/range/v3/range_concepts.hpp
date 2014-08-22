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
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/logical_ops.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct writable_range_traits
            {
                // Associated types
                template<typename T>
                using iterator_t = decltype(begin(std::declval<T>()));

                template<typename T>
                using sentinel_t = decltype(end(std::declval<T>()));

                template<typename T>
                using difference_t = concepts::WeaklyIncrementable::difference_t<iterator_t<T>>;
            };

            struct readable_range_traits : virtual writable_range_traits
            {
                // Associated types
                template<typename T>
                using category_t = concepts::WeakInputIterator::category_t<iterator_t<T>>;

                template<typename T>
                using value_t = concepts::Readable::value_t<iterator_t<T>>;

                template<typename T>
                using reference_t = concepts::Readable::reference_t<iterator_t<T>>;

                template<typename T>
                using pointer_t = concepts::Readable::pointer_t<iterator_t<T>>;
            };
        }

        namespace concepts
        {
            struct ConvertibleToRange
              : virtual detail::writable_range_traits
            {
                // Valid expressions
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>(begin(t)),
                        concepts::model_of<Sentinel>(end(t), begin(t))
                    ));
            };

            struct ConvertibleToOutputRange
              : refines<ConvertibleToRange(_1)>
            {
                template<typename T, typename V>
                auto requires_(T && t, V const &v) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<OutputIterator>(begin(t), v)
                    ));
            };

            struct ConvertibleToInputRange
              : refines<ConvertibleToRange>, detail::readable_range_traits
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<InputIterator>(begin(t))
                    ));
            };

            struct ConvertibleToForwardRange
              : refines<ConvertibleToInputRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ForwardIterator>(begin(t))
                    ));
            };

            struct ConvertibleToBidirectionalRange
              : refines<ConvertibleToForwardRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BidirectionalIterator>(begin(t))
                    ));
            };

            struct ConvertibleToRandomAccessRange
              : refines<ConvertibleToBidirectionalRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<RandomAccessIterator>(begin(t))
                    ));
            };

            struct ConvertibleToBoundedRange
              : refines<ConvertibleToRange>
            {
                // Valid expressions
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(begin(t), end(t))
                    ));
            };
        }

        template<typename T>
        using ConvertibleToRange = concepts::models<concepts::ConvertibleToRange, T>;

        template<typename T, typename V>
        using ConvertibleToOutputRange = concepts::models<concepts::ConvertibleToOutputRange, T, V>;

        template<typename T>
        using ConvertibleToInputRange = concepts::models<concepts::ConvertibleToInputRange, T>;

        template<typename T>
        using ConvertibleToForwardRange = concepts::models<concepts::ConvertibleToForwardRange, T>;

        template<typename T>
        using ConvertibleToBidirectionalRange = concepts::models<concepts::ConvertibleToBidirectionalRange, T>;

        template<typename T>
        using ConvertibleToRandomAccessRange = concepts::models<concepts::ConvertibleToRandomAccessRange, T>;

        // Handle sized ranges here:
        #include <range/v3/detail/range_size.hpp>

        namespace concepts
        {
            struct ConvertibleToSizedRange
              : refines<ConvertibleToRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(range_size(t))
                    ));
            };
        }

        template<typename T>
        using ConvertibleToBoundedRange = concepts::models<concepts::ConvertibleToBoundedRange, T>;

        template<typename T>
        using ConvertibleToSizedRange = concepts::models<concepts::ConvertibleToSizedRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // convertible_to_bounded_range_concept
        template<typename T>
        using convertible_to_bounded_range_concept =
            concepts::most_refined<
                typelist<
                    concepts::ConvertibleToBoundedRange,
                    concepts::ConvertibleToRange>, T>;

        template<typename T>
        using convertible_to_bounded_range_concept_t =
            meta_apply<convertible_to_bounded_range_concept, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // convertible_to_sized_range_concept
        template<typename T>
        using convertible_to_sized_range_concept =
            concepts::most_refined<
                typelist<
                    concepts::ConvertibleToSizedRange,
                    concepts::ConvertibleToRange>, T>;

        template<typename T>
        using convertible_to_sized_range_concept_t =
            meta_apply<convertible_to_sized_range_concept, T>;

        #include <range/v3/detail/as_range.hpp>

        namespace concepts
        {
            struct Range
              : virtual detail::writable_range_traits
            {
                template<typename T, typename R = as_range_t<detail::uncvref_t<T>>>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToRange>((T &&) t),
                        concepts::model_of<SemiRegular>((detail::uncvref_t<T>) t),
                        concepts::is_true(std::is_same<detail::uncvref_t<T>, R>())
                    ));
            };

            struct OutputRange
              : refines<Range(_1)>
            {
                template<typename T, typename V>
                auto requires_(T && t, V const &v) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<OutputIterator>(begin(t), v)
                    ));
            };

            struct InputRange
              : refines<Range>, detail::readable_range_traits
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<InputIterator>(begin(t))
                    ));
            };

            struct ForwardRange
              : refines<InputRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ForwardIterator>(begin(t))
                    ));
            };

            struct BidirectionalRange
              : refines<ForwardRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BidirectionalIterator>(begin(t))
                    ));
            };

            struct RandomAccessRange
              : refines<BidirectionalRange>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<RandomAccessIterator>(begin(t))
                    ));
            };

            // Additional concepts for checking additional orthogonal properties
            struct BoundedRange
              : refines<Range>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToBoundedRange>((T &&) t)
                    ));
            };

            struct SizedRange
              : refines<Range>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToSizedRange>((T &&) t)
                    ));
            };
        }

        template<typename T>
        using Range = concepts::models<concepts::Range, T>;

        template<typename T, typename V>
        using OutputRange = concepts::models<concepts::OutputRange, T, V>;

        template<typename T>
        using InputRange = concepts::models<concepts::InputRange, T>;

        template<typename T>
        using ForwardRange = concepts::models<concepts::ForwardRange, T>;

        template<typename T>
        using BidirectionalRange = concepts::models<concepts::BidirectionalRange, T>;

        template<typename T>
        using RandomAccessRange = concepts::models<concepts::RandomAccessRange, T>;

        // Extra concepts:
        template<typename T>
        using BoundedRange = concepts::models<concepts::BoundedRange, T>;

        template<typename T>
        using SizedRange = concepts::models<concepts::SizedRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // bounded_range_concept
        template<typename T>
        using bounded_range_concept =
            concepts::most_refined<
                typelist<
                    concepts::BoundedRange,
                    concepts::Range>, T>;

        template<typename T>
        using bounded_range_concept_t = meta_apply<bounded_range_concept, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // sized_range_concept
        template<typename T>
        using sized_range_concept =
            concepts::most_refined<
                typelist<
                    concepts::SizedRange,
                    concepts::Range>, T>;

        template<typename T>
        using sized_range_concept_t = meta_apply<sized_range_concept, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Some helpers for requirements checking
        template<typename T>
        constexpr bool ConvertibleToSizedBoundedRange()
        {
            return ConvertibleToSizedRange<T>() && ConvertibleToBoundedRange<T>();
        }

        template<typename T, typename V>
        constexpr bool ConvertibleToOutputBoundedRange()
        {
            return ConvertibleToOutputRange<T, V>() && ConvertibleToBoundedRange<T>();
        }

        template<typename T, typename V>
        constexpr bool ConvertibleToOutputSizedRange()
        {
            return ConvertibleToOutputRange<T, V>() && ConvertibleToSizedRange<T>();
        }

        template<typename T, typename V>
        constexpr bool ConvertibleToOutputSizedBoundedRange()
        {
            return ConvertibleToOutputBoundedRange<T, V>() && ConvertibleToSizedBoundedRange<T>();
        }

        template<typename T>
        constexpr bool SizedBoundedRange()
        {
            return SizedRange<T>() && BoundedRange<T>();
        }

        template<typename T, typename V>
        constexpr bool OutputBoundedRange()
        {
            return OutputRange<T, V>() && BoundedRange<T>();
        }

        template<typename T, typename V>
        constexpr bool OutputSizedRange()
        {
            return OutputRange<T, V>() && SizedRange<T>();
        }

        template<typename T, typename V>
        constexpr bool OutputSizedBoundedRange()
        {
            return OutputBoundedRange<T, V>() && SizedBoundedRange<T>();
        }

    #define RANGES_RANGE_CONCEPTS_DEF(Category)                         \
        template<typename T>                                            \
        constexpr bool ConvertibleTo ## Category ## BoundedRange()      \
        {                                                               \
            return ConvertibleTo ## Category ## Range<T>() &&           \
                   ConvertibleTo ## BoundedRange<T>();                  \
        }                                                               \
        template<typename T>                                            \
        constexpr bool ConvertibleTo ## Category ## SizedRange()        \
        {                                                               \
            return ConvertibleTo ## Category ## Range<T>() &&           \
                   ConvertibleTo ## SizedRange<T>();                    \
        }                                                               \
        template<typename T>                                            \
        constexpr bool ConvertibleTo ## Category ## SizedBoundedRange() \
        {                                                               \
            return ConvertibleTo ## Category ## SizedRange<T>() &&      \
                   ConvertibleTo ## BoundedRange<T>();                  \
        }                                                               \
        template<typename T>                                            \
        constexpr bool Category ## BoundedRange()                       \
        {                                                               \
            return Category ## Range<T>() && BoundedRange<T>();         \
        }                                                               \
        template<typename T>                                            \
        constexpr bool Category ## SizedRange()                         \
        {                                                               \
            return Category ## Range<T>() && SizedRange<T>();           \
        }                                                               \
        template<typename T>                                            \
        constexpr bool Category ## SizedBoundedRange()                  \
        {                                                               \
            return Category ## SizedRange<T>() && BoundedRange<T>();    \
        }                                                               \
        /**/

        RANGES_RANGE_CONCEPTS_DEF(Input)
        RANGES_RANGE_CONCEPTS_DEF(Forward)
        RANGES_RANGE_CONCEPTS_DEF(Bidirectional)
        RANGES_RANGE_CONCEPTS_DEF(RandomAccess)
    }
}

#endif
