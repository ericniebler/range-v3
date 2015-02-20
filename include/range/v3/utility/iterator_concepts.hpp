/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
#define RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP

#include <iterator>
#include <type_traits>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        struct weak_input_iterator_tag
        {};

        struct input_iterator_tag
          : weak_input_iterator_tag
        {};

        struct forward_iterator_tag
          : input_iterator_tag
        {};

        struct bidirectional_iterator_tag
          : forward_iterator_tag
        {};

        struct random_access_iterator_tag
          : bidirectional_iterator_tag
        {};
        /// @}

        /// \cond
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
            template<typename Tag, typename Reference>
            struct as_std_iterator_category;

            template<typename Reference>
            struct as_std_iterator_category<ranges::weak_input_iterator_tag, Reference>
            {
                // Not a valid C++14 iterator
            };

            template<typename Reference>
            struct as_std_iterator_category<ranges::input_iterator_tag, Reference>
            {
                using type = std::input_iterator_tag;
            };

            template<typename Reference>
            struct as_std_iterator_category<ranges::forward_iterator_tag, Reference>
            {
                using type = meta::if_<
                    std::is_reference<Reference>,
                    std::forward_iterator_tag,
                    std::input_iterator_tag>;
            };

            template<typename Reference>
            struct as_std_iterator_category<ranges::bidirectional_iterator_tag, Reference>
            {
                using type = meta::if_<
                    std::is_reference<Reference>,
                    std::bidirectional_iterator_tag,
                    std::input_iterator_tag>;
            };

            template<typename Reference>
            struct as_std_iterator_category<ranges::random_access_iterator_tag, Reference>
            {
                using type = meta::if_<
                    std::is_reference<Reference>,
                    std::random_access_iterator_tag,
                    std::input_iterator_tag>;
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
            struct iterator_category_type<T, meta::void_<typename T::iterator_category>>
              : as_ranges_iterator_category<typename T::iterator_category>
            {};
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        template<typename T>
        struct iterator_category_type
          : detail::iterator_category_type<uncvref_t<T>>
        {};

        namespace concepts
        {
            struct Readable
              : refines<SemiRegular>
            {
                // Associated types
                template<typename I>
                using value_t = meta::eval<value_type<I>>;

                template<typename I>
                using reference_t = decltype(*std::declval<I>());

                template<typename I>
                using rvalue_reference_t = decltype(indirect_move(std::declval<I>()));

                template<typename I>
                using common_reference_t =
                    ranges::common_reference_t<reference_t<I> &&, value_t<I> &>;

                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        // The value, reference and rvalue reference types are related
                        // through the CommonReference concept.
                        concepts::model_of<CommonReference, reference_t<I> &&, value_t<I> &>(),
                        concepts::model_of<CommonReference, reference_t<I> &&, rvalue_reference_t<I> &&>(),
                        concepts::model_of<CommonReference, rvalue_reference_t<I> &&, value_t<I> const &>(),
                        // Experimental additional tests. If nothing else, this is a good workout
                        // for the common_reference code.
                        concepts::model_of<Same, ranges::common_reference_t<reference_t<I>, value_t<I>>, value_t<I>>(),
                        concepts::model_of<Same, ranges::common_reference_t<rvalue_reference_t<I>, value_t<I>>, value_t<I>>()
                    ));
            };

            struct MoveWritable
              : refines<SemiRegular(_1)>
            {
                template<typename Out, typename T>
                auto requires_(Out o, T) -> decltype(
                    concepts::valid_expr(
                        *o = std::move(val<T>())
                    ));
            };

            struct Writable
              : refines<MoveWritable>
            {
                template<typename Out, typename T>
                auto requires_(Out o, T) -> decltype(
                    concepts::valid_expr(
                        *o = val<T>()
                    ));
            };

            struct IndirectlyMovable
            {
                template<typename I, typename O>
                auto requires_(I i, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable, I>(),
                        concepts::model_of<SemiRegular, O>(),
                        concepts::model_of<Convertible, Readable::rvalue_reference_t<I> &&,
                            Readable::value_t<I>>(),
                        concepts::model_of<MoveWritable, O, Readable::rvalue_reference_t<I> &&>(),
                        concepts::model_of<MoveWritable, O, Readable::value_t<I> &&>()
                    ));
            };

            // BUGBUG shouldn't this also require that I's value type is Semiregular?
            // See unique_copy for InputIterators.
            struct IndirectlyCopyable
              : refines<IndirectlyMovable>
            {
                template<typename I, typename O>
                auto requires_(I i, O o) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable, I>(),
                        concepts::model_of<SemiRegular, O>(),
                        concepts::model_of<Convertible, Readable::reference_t<I> &&,
                            Readable::value_t<I>>(),
                        concepts::model_of<Writable, O, Readable::reference_t<I> &&>(),
                        concepts::model_of<Writable, O, Readable::common_reference_t<I> &&>(),
                        concepts::model_of<Writable, O, Readable::value_t<I> const &>()
                    ));
            };

            struct IndirectlySwappable
            {
                template<typename I1, typename I2>
                auto requires_(I1 i1, I2 i2) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable, I1>(),
                        concepts::model_of<Readable, I2>(),
                        (ranges::indirect_swap(i1, i2), 42),
                        (ranges::indirect_swap(i1, i1), 42),
                        (ranges::indirect_swap(i2, i2), 42),
                        (ranges::indirect_swap(i2, i1), 42)
                    ));
            };

            struct WeaklyIncrementable
              : refines<SemiRegular>
            {
                // Associated types
                template<typename I>
                using difference_t = meta::eval<difference_type<I>>;

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
              : refines<WeakIterator, EqualityComparable>
            {};

            struct WeakOutputIterator
              : refines<WeakIterator(_1), Writable>
            {};

            struct OutputIterator
              : refines<WeakOutputIterator, Iterator(_1)>
            {};

            struct WeakInputIterator
              : refines<WeakIterator, Readable>
            {
                // Associated types
                // value_t from Readable
                // distance_t from WeaklyIncrementable
                template<typename I>
                using category_t = meta::eval<ranges::iterator_category_type<I>>;

                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived, category_t<I>, ranges::weak_input_iterator_tag>(),
                        concepts::model_of<Readable>(i++)
                    ));
            };

            struct InputIterator
              : refines<WeakInputIterator, Iterator, EqualityComparable>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived, category_t<I>, ranges::input_iterator_tag>()
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator, Incrementable>
            {
                template<typename I>
                auto requires_(I) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived, category_t<I>, ranges::forward_iterator_tag>()
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived, category_t<I>, ranges::bidirectional_iterator_tag>(),
                        concepts::has_type<I &>(--i),
                        concepts::has_type<I>(i--),
                        concepts::same_type(*i, *i--)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator, TotallyOrdered>
            {
                template<typename I, typename V = common_reference_t<I>>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Derived, category_t<I>, ranges::random_access_iterator_tag>(),
                        concepts::model_of<SignedIntegral>(i - i),
                        concepts::has_type<difference_t<I>>(i - i),
                        concepts::has_type<I>(i + (i - i)),
                        concepts::has_type<I>((i - i) + i),
                        concepts::has_type<I>(i - (i - i)),
                        concepts::has_type<I &>(i += (i-i)),
                        concepts::has_type<I &>(i -= (i - i)),
                        // BUGBUG Should be CommonReference<V const &, decltype(i[i-i])>
                        // Redesign basic_iterator's operator[]'s proxy reference type
                        concepts::convertible_to<V>(i[i - i])
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

        template<typename I1, typename I2>
        using IndirectlySwappable = concepts::models<concepts::IndirectlySwappable, I1, I2>;

        template<typename T>
        using WeaklyIncrementable = concepts::models<concepts::WeaklyIncrementable, T>;

        template<typename T>
        using Incrementable = concepts::models<concepts::Incrementable, T>;

        template<typename I>
        using WeakIterator = concepts::models<concepts::WeakIterator, I>;

        template<typename I>
        using Iterator = concepts::models<concepts::Iterator, I>;

        template<typename Out, typename T>
        using WeakOutputIterator = concepts::models<concepts::WeakOutputIterator, Out, T>;

        template<typename Out, typename T>
        using OutputIterator = concepts::models<concepts::OutputIterator, Out, T>;

        template<typename I>
        using WeakInputIterator = concepts::models<concepts::WeakInputIterator, I>;

        template<typename I>
        using InputIterator = concepts::models<concepts::InputIterator, I>;

        template<typename I>
        using ForwardIterator = concepts::models<concepts::ForwardIterator, I>;

        template<typename I>
        using BidirectionalIterator = concepts::models<concepts::BidirectionalIterator, I>;

        template<typename I>
        using RandomAccessIterator = concepts::models<concepts::RandomAccessIterator, I>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator_concept
        template<typename T>
        using iterator_concept =
            concepts::most_refined<
                meta::list<
                    concepts::RandomAccessIterator,
                    concepts::BidirectionalIterator,
                    concepts::ForwardIterator,
                    concepts::InputIterator,
                    concepts::WeakInputIterator>, T>;

        template<typename T>
        using iterator_concept_t = meta::eval<iterator_concept<T>>;

        // Generally useful to know if an iterator is single-pass or not:
        template<typename I>
        using SinglePass = meta::fast_and<WeakInputIterator<I>, meta::not_<ForwardIterator<I>>>;

        namespace detail
        {
            template<typename I, typename Proj>
            struct projected_readable
            {
                using value_type =
                    decay_t<concepts::Invokable::result_t<Proj, concepts::Readable::value_t<I>>>;
                using reference =
                    concepts::Invokable::result_t<Proj, concepts::Readable::reference_t<I>>;
                reference operator*() const;
                friend auto indirect_move(projected_readable const &) ->
                    concepts::Invokable::result_t<Proj, concepts::Readable::rvalue_reference_t<I>>
                {
                    RANGES_ASSERT(false);
                    throw;
                }
            };

            template<typename I, typename Proj>
            struct Projectable_
            {
                using type =
                    meta::fast_and<
                        Invokable<Proj, concepts::Readable::value_t<I>>,
                        Invokable<Proj, concepts::Readable::reference_t<I>>,
                        Invokable<Proj, concepts::Readable::rvalue_reference_t<I>>>;
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Composite concepts for use defining algorithms:
        template<typename I, typename Proj>
        using Projectable = meta::and_<
            Readable<I>,
            detail::Projectable_<I, Proj>>;

        template<typename I, typename Proj>
        using Project =
            meta::eval<std::enable_if<
                Projectable<I, Proj>::value,
                detail::projected_readable<I, Proj>>>;

        namespace detail
        {
            // Return the value and reference types of an iterator in a list.
            template<typename I>
            using readable_types =
                meta::list<concepts::Readable::value_t<I>, concepts::Readable::reference_t<I>>;

            template<typename CombineFn, typename ApplyFn, typename...Is>
            using indirect_apply_combine =
                // Collect the list of results computed below with CombineFn
                meta::apply_list<
                    CombineFn,
                    // Call ApplyFn with the cartesian product of the Readables' value and reference
                    // types. In addition, call ApplyFn with the common_reference type of all the
                    // Readables. Return all the results as a list.
                    meta::transform<
                        meta::push_back<
                            meta::cartesian_product<
                                meta::transform<meta::list<Is...>, meta::quote<readable_types>>>,
                            meta::list<concepts::Readable::common_reference_t<Is>...>>,
                        meta::bind_front<meta::quote<meta::apply_list>, ApplyFn>>>;
        }

        template<typename C, typename ...Is>
        using IndirectFunction = meta::and_<
            meta::fast_and<Readable<Is>...>,
            // C must be callable with the values and references read from the Is.
            meta::lazy_apply<
                meta::quote<detail::indirect_apply_combine>,
                meta::quote<meta::fast_and>,
                meta::bind_front<meta::quote<Function>, C>,
                Is...>,
            // In addition, the return types of the C invocations tried above must all
            // share a common reference type. (The lazy_apply is so that this doesn't get
            // evaluated unless C is truly callable as determined above.)
            meta::lazy_apply<
                meta::quote<detail::indirect_apply_combine>,
                meta::quote<CommonReference>,
                meta::bind_front<meta::quote<concepts::Function::result_t>, C>,
                Is...> >;

        template<typename C, typename ...Is>
        using IndirectPredicate = meta::and_<
            meta::fast_and<Readable<Is>...>,
            meta::lazy_apply<
                meta::quote<detail::indirect_apply_combine>,
                meta::quote<meta::fast_and>,
                meta::bind_front<meta::quote<Predicate>, C>,
                Is...>>;

        template<typename C, typename I0, typename I1 = I0>
        using IndirectRelation = meta::and_<
            meta::fast_and<Readable<I0>, Readable<I1>>,
            meta::lazy_apply<
                meta::quote<detail::indirect_apply_combine>,
                meta::quote<meta::fast_and>,
                meta::bind_front<meta::quote<Relation>, C>,
                I0, I1>>;

        template<typename C, typename ...Is>
        using IndirectInvokable = IndirectFunction<invokable_t<C>, Is...>;

        template<typename C, typename ...Is>
        using IndirectInvokablePredicate = IndirectPredicate<invokable_t<C>, Is...>;

        template<typename C, typename I0, typename I1 = I0>
        using IndirectInvokableRelation = IndirectRelation<invokable_t<C>, I0, I1>;

        template<typename I, typename V = concepts::Readable::value_t<I>>
        using Permutable = meta::fast_and<
            ForwardIterator<I>,
            Movable<V>,
            IndirectlyMovable<I, I>>;

        template<typename I0, typename I1, typename Out, typename C = ordered_less,
            typename P0 = ident, typename P1 = ident>
        using Mergeable = meta::fast_and<
            InputIterator<I0>,
            InputIterator<I1>,
            WeaklyIncrementable<Out>,
            IndirectInvokableRelation<C, Project<I0, P0>, Project<I1, P1>>,
            IndirectlyCopyable<I0, Out>,
            IndirectlyCopyable<I1, Out>>;

        template<typename I0, typename I1, typename Out, typename C = ordered_less,
            typename P0 = ident, typename P1 = ident>
        using MergeMovable = meta::fast_and<
            InputIterator<I0>,
            InputIterator<I1>,
            WeaklyIncrementable<Out>,
            IndirectInvokableRelation<C, Project<I0, P0>, Project<I1, P1>>,
            IndirectlyMovable<I0, Out>,
            IndirectlyMovable<I1, Out>>;

        template<typename I, typename C = ordered_less, typename P = ident>
        using Sortable = meta::fast_and<
            ForwardIterator<I>,
            IndirectInvokableRelation<C, Project<I, P>, Project<I, P>>,
            Permutable<I>>;

        template<typename I, typename V2, typename C = ordered_less, typename P = ident>
        using BinarySearchable = meta::fast_and<
            ForwardIterator<I>,
            TotallyOrdered<V2>,
            IndirectInvokableRelation<C, Project<I, P>, V2 const *>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using WeaklyAsymmetricallyComparable = meta::fast_and<
            InputIterator<I1>,
            WeakInputIterator<I2>,
            IndirectInvokablePredicate<C, Project<I1, P1>, Project<I2, P2>>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using AsymmetricallyComparable = meta::fast_and<
            WeaklyAsymmetricallyComparable<I1, I2, C, P1, P2>,
            InputIterator<I2>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using WeaklyComparable = meta::fast_and<
            WeaklyAsymmetricallyComparable<I1, I2, C, P1, P2>,
            IndirectInvokableRelation<C, Project<I1, P1>, Project<I2, P2>>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Comparable = meta::fast_and<
            WeaklyComparable<I1, I2, C, P1, P2>,
            InputIterator<I2>>;

        namespace concepts
        {
            struct IteratorRange
            {
                template<typename I, typename S>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeakIterator, I>(),
                        concepts::model_of<Regular, S>(),
                        concepts::model_of<EqualityComparable, I, S>()
                    ));
            };

            // Detail, used only to constrain common_iterator::operator-, which
            // is used by SizedIteratorRange
            struct SizedIteratorRangeLike_
              : refines<IteratorRange>
            {
              template<typename I, typename S>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(s - i),
                        concepts::same_type(s - i, i - s)
                    ));
            };

            struct SizedIteratorRange
              : refines<IteratorRange>
            {
                template<typename I, typename S,
                    enable_if_t<std::is_same<I, S>::value> = 0>
                auto requires_(I i, I s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(s - i)
                    ));

                template<typename I, typename S,
                    enable_if_t<!std::is_same<I, S>::value> = 0,
                    typename C = common_type_t<I, S>>
                auto requires_(I i, S s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<SizedIteratorRange, I, I>(),
                        concepts::model_of<Common, I, S>(),
                        concepts::model_of<SizedIteratorRange, C, C>(),
                        concepts::model_of<Integral>(s - i),
                        concepts::same_type(s - i, i - s)
                    ));
            };
        }

        template<typename I, typename S>
        using IteratorRange = concepts::models<concepts::IteratorRange, I, S>;

        template<typename I, typename S>
        using SizedIteratorRangeLike_ = concepts::models<concepts::SizedIteratorRangeLike_, I, S>;

        template<typename I, typename S>
        using SizedIteratorRange = concepts::models<concepts::SizedIteratorRange, I, S>;

        template<typename I, typename S = I>
        using sized_iterator_range_concept =
            concepts::most_refined<
                meta::list<
                    concepts::SizedIteratorRange,
                    concepts::IteratorRange>, I, S>;

        template<typename I, typename S = I>
        using sized_iterator_range_concept_t = meta::eval<sized_iterator_range_concept<I, S>>;
        /// @}
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
