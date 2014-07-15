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
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

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
            struct as_ranges_iterator_category
            {
                using type = T;
            };

            template<>
            struct as_ranges_iterator_category<std::input_iterator_tag>
            {
                using type = ranges::input_iterator_tag;
            };

            template<>
            struct as_ranges_iterator_category<std::forward_iterator_tag>
            {
                using type = ranges::forward_iterator_tag;
            };

            template<>
            struct as_ranges_iterator_category<std::bidirectional_iterator_tag>
            {
                using type = ranges::bidirectional_iterator_tag;
            };

            template<>
            struct as_ranges_iterator_category<std::random_access_iterator_tag>
            {
                using type = ranges::random_access_iterator_tag;
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T>
            struct as_std_iterator_category;

            template<>
            struct as_std_iterator_category<ranges::weak_input_iterator_tag>
            {
                using type = std::input_iterator_tag;
            };

            template<>
            struct as_std_iterator_category<ranges::input_iterator_tag>
            {
                using type = std::input_iterator_tag;
            };

            template<>
            struct as_std_iterator_category<ranges::forward_iterator_tag>
            {
                using type = std::forward_iterator_tag;
            };

            template<>
            struct as_std_iterator_category<ranges::bidirectional_iterator_tag>
            {
                using type = std::bidirectional_iterator_tag;
            };

            template<>
            struct as_std_iterator_category<ranges::random_access_iterator_tag>
            {
                using type = std::random_access_iterator_tag;
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
              : as_ranges_iterator_category<typename T::iterator_category>
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
                using value_t = meta_apply<value_type, I>;

                template<typename I>
                using reference_t = decltype(*std::declval<I>());

                template<typename I>
                using pointer_t = meta_apply<pointer_type, I>;

                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<value_t<I> const &>(*i)
                    ));
            };

            struct MoveWritable
              : refines<SemiRegular(_1)>
            {
                template<typename Out, typename T>
                auto requires_(Out o, T && value) -> decltype(
                    concepts::valid_expr(
                        *o = std::move(value)
                    ));
            };

            struct Writable
              : refines<MoveWritable>
            {
                template<typename Out, typename T>
                auto requires_(Out o, T const &value) -> decltype(
                    concepts::valid_expr(
                        *o = value
                    ));
            };

            struct IndirectlyMovable
            {
                template<typename I, typename O>
                auto requires_(I i, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable>((I) i),
                        concepts::model_of<SemiRegular>((O) o),
                        concepts::model_of<MoveWritable>((O) o, *i)
                    ));
            };

            struct IndirectlyCopyable
            {
                template<typename I, typename O>
                auto requires_(I i, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable>((I) i),
                        concepts::model_of<SemiRegular>((O) o),
                        concepts::model_of<Writable>((O) o, *i)
                    ));
            };

            struct IndirectlyProjectedMovable
            {
                template<typename I, typename P, typename O>
                auto requires_(I i, P && p, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable>((I) i),
                        concepts::model_of<RegularInvokable>((P &&) p, *i),
                        concepts::model_of<SemiRegular>((O) o),
                        concepts::model_of<MoveWritable>((O) o, invokable((P &&) p)(*i))
                    ));
            };

            struct IndirectlyProjectedCopyable
            {
                template<typename I, typename P, typename O>
                auto requires_(I i, P && p, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable>((I) i),
                        concepts::model_of<RegularInvokable>((P &&) p, *i),
                        concepts::model_of<SemiRegular>((O) o),
                        concepts::model_of<Writable>((O) o, invokable((P &&) p)(*i))
                    ));
            };

            struct WeaklyIncrementable
              : refines<SemiRegular>
            {
                // Associated types
                template<typename I>
                using difference_t = meta_apply<difference_type, I>;

                template<typename I>
                auto requires_(I i) -> decltype(
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
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<I>(i++)
                    ));
            };

            struct Sentinel;

            struct WeakIterator
              : refines<WeaklyIncrementable, Copyable>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        *i
                    ));
            };

            struct Iterator
              : refines<WeakIterator(_1), EqualityComparable>
            {
                template<typename I>
                void requires_(I i);

                template<typename I, typename S>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Sentinel>((S) s, (I) i)
                    ));
            };

            struct WeakOutputIterator
              : refines<WeakIterator(_1), Writable>
            {};

            // BUGBUG OutputIterator refines Iterator, but OutputIterator<A,B,C> should
            // refine Iterator<A,C>, where C is an optional parameter. The concept checking
            // code makes that impossible to express currently.
            struct OutputIterator
              : refines<WeakOutputIterator(_1, _2), Iterator(_1)>
            {
                template<typename O, typename T>
                auto requires_(O o, T const &) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<EqualityComparable>((O) o)
                    ));

                template<typename O, typename T, typename S>
                auto requires_(O o, T const &t, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<OutputIterator>((O) o, t),
                        concepts::model_of<Sentinel>((S) s, (O) o)
                    ));
            };

            struct WeakInputIterator
              : refines<WeakIterator, Readable>
            {
                // Associated types
                // value_t from readable
                // distance_t from WeaklyIncrementable
                template<typename I>
                using category_t = meta_apply<ranges::iterator_category_type, I>;

                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::weak_input_iterator_tag{}),
                        concepts::model_of<Readable>(i++)
                    ));
            };

            struct InputIterator
              : refines<WeakInputIterator(_1), Iterator(_1), EqualityComparable>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::input_iterator_tag{})
                    ));

                template<typename I, typename S>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<InputIterator>((I) i),
                        concepts::model_of<Sentinel>((S) s, (I) i)
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator, Incrementable(_1)>
            {
                template<typename I>
                auto requires_(I) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::forward_iterator_tag{})
                    ));

                template<typename I, typename S>
                auto requires_(I i, S) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ForwardIterator>((I) i)
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived>(category_t<I>{}, ranges::bidirectional_iterator_tag{}),
                        concepts::has_type<I &>(--i),
                        concepts::has_type<I>(i--),
                        concepts::same_type(*i, *i--)
                    ));

                template<typename I, typename S>
                auto requires_(I i, S) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BidirectionalIterator>((I) i)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator, TotallyOrdered(_1)>
            {
                template<typename I, typename V = value_t<I>>
                auto requires_(I i) -> decltype(
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

                template<typename I, typename S>
                auto requires_(I i, S) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<RandomAccessIterator>((I) i)
                    ));
            };

            struct Sentinel
              : refines<Regular(_1), EqualityComparable(_2, _1)>
            {
                template<typename S, typename I>
                auto requires_(S, I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>((I) i)
                    ));
            };
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

        template<typename I, typename P, typename O>
        using IndirectlyProjectedMovable = concepts::models<concepts::IndirectlyProjectedMovable, I, P, O>;

        template<typename I, typename P, typename O>
        using IndirectlyProjectedCopyable = concepts::models<concepts::IndirectlyProjectedCopyable, I, P, O>;

        template<typename T>
        using WeaklyIncrementable = concepts::models<concepts::WeaklyIncrementable, T>;

        template<typename T>
        using Incrementable = concepts::models<concepts::Incrementable, T>;

        template<typename I>
        using WeakIterator = concepts::models<concepts::WeakIterator, I>;

        template<typename I, typename S = I>
        using Iterator = concepts::models<concepts::Iterator, I, S>;

        template<typename Out, typename T>
        using WeakOutputIterator = concepts::models<concepts::WeakOutputIterator, Out, T>;

        template<typename Out, typename T, typename S = Out>
        using OutputIterator = concepts::models<concepts::OutputIterator, Out, T, S>;

        template<typename I>
        using WeakInputIterator = concepts::models<concepts::WeakInputIterator, I>;

        template<typename I, typename S = I>
        using InputIterator = concepts::models<concepts::InputIterator, I, S>;

        template<typename I, typename S = I>
        using ForwardIterator = concepts::models<concepts::ForwardIterator, I, S>;

        template<typename I, typename S = I>
        using BidirectionalIterator = concepts::models<concepts::BidirectionalIterator, I, S>;

        template<typename I, typename S = I>
        using RandomAccessIterator = concepts::models<concepts::RandomAccessIterator, I, S>;

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

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Composite concepts for use defining algorithms:
        template<typename I, typename V = concepts::Readable::value_t<I>>
        constexpr bool Permutable()
        {
            return ForwardIterator<I>()     &&
                   Movable<V>()             &&
                   IndirectlyMovable<I, I>();
        }

        template<typename I0, typename I1, typename Out, typename C = ordered_less,
            typename P0 = ident, typename P1 = ident,
            typename V0 = concepts::Readable::value_t<I0>,
            typename V1 = concepts::Readable::value_t<I1>,
            typename X0 = concepts::Invokable::result_t<P0, V0>,
            typename X1 = concepts::Invokable::result_t<P1, V1>>
        constexpr bool Mergeable()
        {
            return InputIterator<I0>()              &&
                   InputIterator<I1>()              &&
                   WeaklyIncrementable<Out>()       &&
                   InvokableRelation<C, X1, X0>()   &&
                   IndirectlyCopyable<I0, Out>()    &&
                   IndirectlyCopyable<I1, Out>();
        }

        template<typename I, typename C = ordered_less, typename P = ident,
            typename V = concepts::Readable::value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        constexpr bool Sortable()
        {
            return ForwardIterator<I>()         &&
                   Invokable<P, V>()            &&
                   InvokableRelation<C, X, X>() &&
                   Permutable<I>();
        }

        template<typename I, typename V2, typename R = ordered_less, typename P = ident,
            typename V = concepts::Readable::value_t<I>,
            typename X = concepts::Invokable::result_t<P, V> >
        constexpr bool BinarySearchable()
        {
            return ForwardIterator<I>()            &&
                   Invokable<P, V>()               &&
                   InvokableRelation<R, X, V2>();
        }

        namespace concepts
        {
            struct IteratorRange
            {
                template<typename I, typename S>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Iterator>((I) i, (S) s)
                    ));
            };

            struct SizedIteratorRange
              : refines<IteratorRange>
            {
                template<typename I, typename S>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(s - i)
                    ));
            };
        }

        template<typename I, typename S = I>
        using IteratorRange = concepts::models<concepts::IteratorRange, I, S>;

        template<typename I, typename S = I>
        using SizedIteratorRange = concepts::models<concepts::SizedIteratorRange, I, S>;

        template<typename I, typename S = I>
        using sized_iterator_range_concept =
            concepts::most_refined<
                typelist<
                    concepts::SizedIteratorRange,
                    concepts::IteratorRange>, I, S>;

        template<typename I, typename S = I>
        using sized_iterator_range_concept_t = meta_apply<sized_iterator_range_concept, I, S>;

    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
