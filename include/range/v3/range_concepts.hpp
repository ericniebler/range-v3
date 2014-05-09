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
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

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
            struct ConvertibleToIterable
              : virtual detail::writable_range_traits
            {
                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>(begin(t)),
                        concepts::model_of<Sentinel>(end(t), begin(t))
                    ));
            };

            struct ConvertibleToRange
              : refines<ConvertibleToIterable>
            {
                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(begin(t), end(t))
                    ));
            };
        }

        template<typename T>
        using ConvertibleToIterable = concepts::models<concepts::ConvertibleToIterable, T>;

        template<typename T>
        using ConvertibleToRange = concepts::models<concepts::ConvertibleToRange, T>;

        // Handle sized iterables here:
        #include <range/v3/detail/range_size.hpp>

        namespace concepts
        {
            struct ConvertibleToSizedIterable
              : refines<ConvertibleToIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(range_size(t))
                    ));
            };
        }

        template<typename T>
        using ConvertibleToSizedIterable = concepts::models<concepts::ConvertibleToSizedIterable, T>;

        #include <range/v3/detail/as_iterable.hpp>

        namespace concepts
        {
            struct Iterable
              : virtual detail::writable_range_traits
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToIterable>((T &&) t),
                        concepts::model_of<CopyConstructible>(as_iterable(t))
                    ));
            };

            struct OutputIterable
              : refines<Iterable(_1)>
            {
                template<typename T, typename V>
                auto requires(T && t, V const &v) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<OutputIterator>(begin(t), v)
                    ));
            };

            struct InputIterable
              : refines<Iterable>, detail::readable_range_traits
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<InputIterator>(begin(t))
                    ));
            };

            struct ForwardIterable
              : refines<InputIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ForwardIterator>(begin(t))
                    ));
            };

            struct BidirectionalIterable
              : refines<ForwardIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BidirectionalIterator>(begin(t))
                    ));
            };

            struct RandomAccessIterable
              : refines<BidirectionalIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<RandomAccessIterator>(begin(t))
                    ));
            };

            // Additional concepts for checking additional orthogonal properties
            struct Range
              : refines<Iterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToRange>((T &&) t)
                    ));
            };

            struct SizedIterable
              : refines<Iterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToSizedIterable>((T &&) t)
                    ));
            };
        }

        template<typename T>
        using Iterable = concepts::models<concepts::Iterable, T>;

        template<typename T, typename V>
        using OutputIterable = concepts::models<concepts::OutputIterable, T, V>;

        template<typename T>
        using InputIterable = concepts::models<concepts::InputIterable, T>;

        template<typename T>
        using ForwardIterable = concepts::models<concepts::ForwardIterable, T>;

        template<typename T>
        using BidirectionalIterable = concepts::models<concepts::BidirectionalIterable, T>;

        template<typename T>
        using RandomAccessIterable = concepts::models<concepts::RandomAccessIterable, T>;

        // Extra concepts:
        template<typename T>
        using Range = concepts::models<concepts::Range, T>;

        template<typename T>
        using SizedIterable = concepts::models<concepts::SizedIterable, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // range_concept
        template<typename T>
        using range_concept =
            concepts::most_refined<
                typelist<
                    concepts::Range,
                    concepts::Iterable>, T>;

        template<typename T>
        using range_concept_t = meta_apply<range_concept, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // sized_iterable_concept
        template<typename T>
        using sized_iterable_concept =
            concepts::most_refined<
                typelist<
                    concepts::SizedIterable,
                    concepts::Iterable>, T>;

        template<typename T>
        using sized_iterable_concept_t = meta_apply<sized_iterable_concept, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Some helpers for requirements checking
        template<typename T>
        constexpr bool SizedRange()
        {
            return SizedIterable<T>() && Range<T>();
        }

        template<typename T, typename V>
        constexpr bool OutputRange()
        {
            return OutputIterable<T, V>() && Range<T>();
        }

        template<typename T, typename V>
        constexpr bool OutputSizedIterable()
        {
            return OutputIterable<T, V>() && SizedIterable<T>();
        }

        template<typename T, typename V>
        constexpr bool OutputSizedRange()
        {
            return OutputIterable<T, V>() && SizedRange<T>();
        }

    #define RANGES_RANGE_CONCEPTS_DEF(Category)                         \
        template<typename T>                                            \
        constexpr bool Category ## Range()                              \
        {                                                               \
            return Category ## Iterable<T>() && Range<T>();             \
        }                                                               \
        template<typename T>                                            \
        constexpr bool Category ## SizedIterable()                      \
        {                                                               \
            return Category ## Iterable<T>() && SizedIterable<T>();     \
        }                                                               \
        template<typename T>                                            \
        constexpr bool Category ## SizedRange()                         \
        {                                                               \
            return Category ## SizedIterable<T>() && Range<T>();        \
        }                                                               \
        /**/

        RANGES_RANGE_CONCEPTS_DEF(Input)
        RANGES_RANGE_CONCEPTS_DEF(Forward)
        RANGES_RANGE_CONCEPTS_DEF(Bidirectional)
        RANGES_RANGE_CONCEPTS_DEF(RandomAccess)
    }
}

#endif
