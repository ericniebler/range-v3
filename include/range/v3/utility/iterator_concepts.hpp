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

#ifndef RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
#define RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP

#include <iterator>
#include <type_traits>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct weak_input_iterator_tag
        {};

        struct input_iterator_tag
          : weak_input_iterator_tag
          , virtual std::input_iterator_tag
        {};

        struct forward_iterator_tag
          : input_iterator_tag
          , virtual std::forward_iterator_tag
        {};

        struct bidirectional_iterator_tag
          : forward_iterator_tag
          , virtual std::bidirectional_iterator_tag
        {};

        struct random_access_iterator_tag
          : bidirectional_iterator_tag
          , virtual std::random_access_iterator_tag
        {};

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T>
            struct as_iterator_category
            {
                using type = T;
            };

            template<>
            struct as_iterator_category<std::input_iterator_tag>
            {
                using type = ranges::input_iterator_tag;
            };

            template<>
            struct as_iterator_category<std::forward_iterator_tag>
            {
                using type = ranges::forward_iterator_tag;
            };

            template<>
            struct as_iterator_category<std::bidirectional_iterator_tag>
            {
                using type = ranges::bidirectional_iterator_tag;
            };

            template<>
            struct as_iterator_category<std::random_access_iterator_tag>
            {
                using type = ranges::random_access_iterator_tag;
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename Enable = void>
            struct pointer_type
            {};

            template<typename T>
            struct pointer_type<T *, void>
            {
                using type = T *;
            };

            template<typename T>
            struct pointer_type<T, enable_if_t<std::is_class<T>::value, void>>
            {
                using type = decltype(std::declval<T>().operator->());
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename Enable = void>
            struct iterator_category_type
            {};

            template<typename T>
            struct iterator_category_type<T *, void>
            {
                using type = ranges::random_access_iterator_tag;
            };

            template<typename T>
            struct iterator_category_type<T, always_t<void, typename T::iterator_category>>
              : as_iterator_category<typename T::iterator_category>
            {};
        }

        template<typename T>
        struct pointer_type
          : detail::pointer_type<detail::uncvref_t<T>>
        {};

        template<typename T>
        struct iterator_category_type
          : detail::iterator_category_type<detail::uncvref_t<T>>
        {};

        namespace concepts
        {
            struct Readable
              : refines<SemiRegular>
            {
                // Associated types
                template<typename I>
                using value_t = meta_apply<ranges::value_type, I>;

                template<typename I>
                using reference_t = decltype(*std::declval<I>());

                template<typename I>
                using pointer_t = meta_apply<ranges::pointer_type, I>;

                template<typename I>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<value_t<I> const &>(*i)
                    ));
            };

            struct MoveWritable
              : refines<SemiRegular(_1)>
            {
                template<typename Out, typename T>
                auto requires(Out o, T && value) -> decltype(
                    concepts::valid_expr(
                        *o = std::move(value)
                    ));
            };

            struct Writable
              : refines<MoveWritable>
            {
                template<typename Out, typename T>
                auto requires(Out o, T const &value) -> decltype(
                    concepts::valid_expr(
                        *o = value
                    ));
            };

            struct IndirectlyMovable
            {
                template<typename I, typename O>
                auto requires(I i, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable>(i),
                        concepts::model_of<SemiRegular>(o),
                        concepts::model_of<MoveWritable>(*i, o)
                    ));
            };

            struct IndirectlyCopyable
            {
                template<typename I, typename O>
                auto requires(I i, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable>(i),
                        concepts::model_of<SemiRegular>(o),
                        concepts::model_of<Writable>(*i, o)
                    ));
            };

            struct WeaklyIncrementable
              : refines<SemiRegular>
            {
                // Associated types
                template<typename I>
                using difference_t = meta_apply<ranges::difference_type, I>;

                template<typename I>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<difference_t<I>>{}),
                        concepts::has_type<I &>(++i),
                        ((i++), 42)
                    ));
            };

            struct Incrementable
              : refines<Regular, WeaklyIncrementable>
            {
                template<typename I>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<I>(i++)
                    ));
            };

            struct WeakInputIterator
              : refines<WeaklyIncrementable, Readable>
            {
                // Associated types
                // value_t from readable
                // distance_t from WeaklyIncrementable
                template<typename I>
                using category_t = meta_apply<ranges::iterator_category_type, I>;

                template<typename I>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::weak_input_iterator_tag{}),
                        concepts::model_of<Readable>(i++)
                    ));
            };

            struct WeakOutputIterator
              : refines<WeaklyIncrementable(_1), Writable>
            {};

            struct OutputIterator
              : refines<WeakOutputIterator, EqualityComparable(_1)>
            {};

            struct InputIterator
              : refines<WeakInputIterator, EqualityComparable>
            {
                template<typename I>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::input_iterator_tag{})
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator, Incrementable>
            {
                template<typename I>
                auto requires(I) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::forward_iterator_tag{})
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename I>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::bidirectional_iterator_tag{}),
                        concepts::has_type<I &>(--i),
                        concepts::has_type<I>(i--),
                        concepts::same_type(*i, *i--)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator, TotallyOrdered>
            {
                template<typename I, typename V = value_t<I>>
                auto requires(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::random_access_iterator_tag{}),
                        concepts::model_of<SignedIntegral>(i - i),
                        concepts::has_type<difference_t<I>>(i - i),
                        concepts::has_type<I>(i + (i - i)),
                        concepts::has_type<I>((i - i) + i),
                        concepts::has_type<I>(i - (i - i)),
                        concepts::has_type<I &>(i += (i-i)),
                        concepts::has_type<I &>(i -= (i - i)),
                        concepts::convertible_to<V>(i[i - i])
                    ));
            };

            struct Sentinel
              : refines<CopyConstructible(_1), WeakInputIterator(_2), EqualityComparable>
            {};
        }

        template<typename T>
        using Readable = concepts::models<concepts::Readable, T>;

        template<typename Out, typename T>
        using MoveWritable = concepts::models<concepts::MoveWritable, Out, T>;

        template<typename Out, typename T>
        using Writable = concepts::models<concepts::Writable, Out, T>;

        template<typename I, typename O>
        using IndirectlyMovable = concepts::models<concepts::IndirectlyMovable, I, O>;

        template<typename I, typename O>
        using IndirectlyCopyable = concepts::models<concepts::IndirectlyCopyable, I, O>;

        template<typename T>
        using WeaklyIncrementable = concepts::models<concepts::WeaklyIncrementable, T>;

        template<typename T>
        using Incrementable = concepts::models<concepts::Incrementable, T>;

        template<typename T>
        using WeakInputIterator = concepts::models<concepts::WeakInputIterator, T>;

        template<typename Out, typename T>
        using WeakOutputIterator = concepts::models<concepts::WeakOutputIterator, Out, T>;

        template<typename Out, typename T>
        using OutputIterator = concepts::models<concepts::OutputIterator, Out, T>;

        template<typename T>
        using InputIterator = concepts::models<concepts::InputIterator, T>;

        template<typename T>
        using ForwardIterator = concepts::models<concepts::ForwardIterator, T>;

        template<typename T>
        using BidirectionalIterator = concepts::models<concepts::BidirectionalIterator, T>;

        template<typename T>
        using RandomAccessIterator = concepts::models<concepts::RandomAccessIterator, T>;

        template<typename S, typename I>
        using Sentinel = concepts::models<concepts::Sentinel, S, I>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator_concept
        template<typename T>
        using iterator_concept =
            concepts::most_refined<
                typelist<
                    concepts::RandomAccessIterator,
                    concepts::BidirectionalIterator,
                    concepts::ForwardIterator,
                    concepts::InputIterator,
                    concepts::WeakInputIterator>, T>;

        template<typename T>
        using iterator_concept_t = meta_apply<iterator_concept, T>;
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
