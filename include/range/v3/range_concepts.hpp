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
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

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

                // Valid expressions
                template<typename T,
                    typename CI = iterator_t<detail::as_cref_t<T>>, // const iterator
                    typename CS = sentinel_t<detail::as_cref_t<T>>> // const sentinel
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>(ranges::begin(t)),
                        concepts::model_of<Iterator>(ranges::cbegin(t)),
                        concepts::convertible_to<CI>(ranges::begin(t)),
                        concepts::convertible_to<CS>(ranges::end(t)),
                        concepts::model_of<Sentinel>(ranges::end(t), ranges::begin(t)),
                        concepts::model_of<Sentinel>(ranges::cend(t), ranges::cbegin(t))
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
        }

        template<typename T>
        using Iterable = concepts::models<concepts::Iterable, T>;

        template<typename T>
        using Range = concepts::models<concepts::Range, T>;

        // Handle sized iterables here:
        #include <range/v3/detail/range_size.hpp>

        namespace concepts
        {
            struct SizedIterable
              : refines<Iterable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(range_size(t))
                    ));
            };

            struct SizedRange
              : refines<Range, SizedIterable>
            {};
        }

        template<typename T>
        using SizedIterable = concepts::models<concepts::SizedIterable, T>;

        template<typename T>
        using SizedRange = concepts::models<concepts::SizedRange, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // range_concept
        template<typename T>
        using range_concept_t = concepts::most_refined_t<concepts::SizedRange, T>;

        template<typename T>
        struct range_concept
        {
            using type = range_concept_t<T>;
        };
    }
}

#endif
