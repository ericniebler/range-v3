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
            struct range_traits
            {
                // Associated types
                template<typename T>
                using iterator_t = decltype(ranges::begin(std::declval<T>()));

                template<typename T>
                using sentinel_t = decltype(ranges::end(std::declval<T>()));

                template<typename T>
                using category_t = concepts::WeakInputIterator::category_t<iterator_t<T>>;

                template<typename T>
                using value_t = concepts::Readable::value_t<iterator_t<T>>;

                template<typename T>
                using difference_t = concepts::WeaklyIncrementable::difference_t<iterator_t<T>>;

                template<typename T>
                using reference_t = concepts::Readable::reference_t<iterator_t<T>>;

                template<typename T>
                using pointer_t = concepts::Readable::pointer_t<iterator_t<T>>;
            };
        }

        namespace concepts
        {
            struct ConvertibleToIterable
              : detail::range_traits
            {
                // Valid expressions
                template<typename T,
                    typename CI = iterator_t<detail::as_cref_t<T>>, // const iterator
                    typename CS = sentinel_t<detail::as_cref_t<T>>> // const sentinel
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeakInputIterator>(ranges::begin(t)),
                        concepts::model_of<WeakInputIterator>(ranges::cbegin(t)),
                        concepts::convertible_to<CI>(ranges::begin(t)),
                        concepts::convertible_to<CS>(ranges::end(t)),
                        concepts::model_of<Sentinel>(ranges::end(t), ranges::begin(t)),
                        concepts::model_of<Sentinel>(ranges::cend(t), ranges::cbegin(t))
                    ));
            };

            struct ConvertibleToRange
              : refines<ConvertibleToIterable>
            {
                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(ranges::begin(t), ranges::end(t)),
                        concepts::same_type(ranges::cbegin(t), ranges::cend(t))
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

            struct ConvertibleToSizedRange
              : refines<ConvertibleToRange, ConvertibleToSizedIterable>
            {};
        }

        template<typename T>
        using ConvertibleToSizedIterable = concepts::models<concepts::ConvertibleToSizedIterable, T>;

        template<typename T>
        using ConvertibleToSizedRange = concepts::models<concepts::ConvertibleToSizedRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // convertible_to_range_concept
        template<typename T>
        using convertible_to_range_concept =
            concepts::most_refined<
                typelist<
                    concepts::ConvertibleToSizedRange,
                    concepts::ConvertibleToRange,
                    concepts::ConvertibleToSizedIterable,
                    concepts::ConvertibleToIterable>, T>;

        template<typename T>
        using convertible_to_range_concept_t = meta_apply<convertible_to_range_concept, T>;

        #include <range/v3/detail/as_iterable.hpp>

        namespace concepts
        {
            struct Iterable
              : detail::range_traits
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToIterable>((T &&) t),
                        concepts::model_of<CopyConstructible>(ranges::as_iterable(t))
                    ));
            };

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

            struct SizedRange
              : refines<Range, SizedIterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleToSizedRange>((T &&) t)
                    ));
            };
        }

        template<typename T>
        using Iterable = concepts::models<concepts::Iterable, T>;

        template<typename T>
        using Range = concepts::models<concepts::Range, T>;

        template<typename T>
        using SizedIterable = concepts::models<concepts::SizedIterable, T>;

        template<typename T>
        using SizedRange = concepts::models<concepts::SizedRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // range_concept
        template<typename T>
        using range_concept =
            concepts::most_refined<
                typelist<
                    concepts::SizedRange,
                    concepts::Range,
                    concepts::SizedIterable,
                    concepts::Iterable>, T>;

        template<typename T>
        using range_concept_t = meta_apply<range_concept, T>;
    }
}

#endif
