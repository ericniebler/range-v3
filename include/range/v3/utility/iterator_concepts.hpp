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
#include <meta/meta.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/associated_types.hpp>
#include <range/v3/utility/nullptr_v.hpp>

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
            T upgrade_iterator_category_(T *, void *);

            template<typename T>
            ranges::input_iterator_tag upgrade_iterator_category_(T *, std::input_iterator_tag *);

            template<typename T>
            ranges::forward_iterator_tag upgrade_iterator_category_(T *, std::forward_iterator_tag *);

            template<typename T>
            ranges::bidirectional_iterator_tag upgrade_iterator_category_(T *, std::bidirectional_iterator_tag *);

            template<typename T>
            ranges::random_access_iterator_tag upgrade_iterator_category_(T *, std::random_access_iterator_tag *);

            template<typename T>
            struct upgrade_iterator_category
            {
                using type = decltype(detail::upgrade_iterator_category_(_nullptr_v<T>(), _nullptr_v<T>()));
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename B>
            meta::nil_ downgrade_iterator_category_(T *, void *, B);

            template<typename T, typename B>
            meta::id<T>
            downgrade_iterator_category_(T *, std::input_iterator_tag *, B);

            template<typename T, typename B>
            meta::id<T>
            downgrade_iterator_category_(T *, std::output_iterator_tag *, B);

            template<typename T, typename B>
            meta::id<std::input_iterator_tag>
            downgrade_iterator_category_(T *, ranges::input_iterator_tag *, B);

            template<typename T>
            meta::id<std::forward_iterator_tag>
            downgrade_iterator_category_(T *, ranges::forward_iterator_tag *, std::true_type);

            template<typename T>
            meta::id<std::bidirectional_iterator_tag>
            downgrade_iterator_category_(T *, ranges::bidirectional_iterator_tag *, std::true_type);

            template<typename T>
            meta::id<std::random_access_iterator_tag>
            downgrade_iterator_category_(T *, ranges::random_access_iterator_tag *, std::true_type);

            template<typename Tag, typename Reference>
            struct downgrade_iterator_category
              : decltype(detail::downgrade_iterator_category_(_nullptr_v<Tag>(), _nullptr_v<Tag>(),
                    std::integral_constant<bool, std::is_reference<Reference>::value>()))
            {};
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        template<typename T, typename /*= void*/>
        struct iterator_category
        {};

        template<typename T>
        struct iterator_category<T *>
          : meta::lazy::if_<std::is_object<T>, ranges::random_access_iterator_tag>
        {};

        template<typename T>
        struct iterator_category<T const>
          : iterator_category<T>
        {};

        template<typename T>
        struct iterator_category<T volatile>
          : iterator_category<T>
        {};

        template<typename T>
        struct iterator_category<T const volatile>
          : iterator_category<T>
        {};

        template<typename T>
        struct iterator_category<T, meta::void_<typename T::iterator_category>>
          : detail::upgrade_iterator_category<typename T::iterator_category>
        {};

        namespace concepts
        {
            struct Readable
              : refines<Movable, DefaultConstructible>
            {
                // Associated types
                template<typename I>
                using value_t = meta::_t<value_type<I>>;

                template<typename I>
                using reference_t = decltype(*std::declval<I &>());

                template<typename I>
                using rvalue_reference_t = decltype(indirect_move(std::declval<I &>()));

                template<typename I>
                using common_reference_t =
                    ranges::common_reference_t<reference_t<I> &&, value_t<I> &>;

                template<typename I>
                auto requires_(I&&) -> decltype(
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
              : refines<Movable(_1), DefaultConstructible(_1)>
            {
                template<typename Out, typename T>
                auto requires_(Out&& o, T&&) -> decltype(
                    concepts::valid_expr(
                        *o = std::move(val<T>())
                    ));
            };

            struct Writable
              : refines<MoveWritable>
            {
                template<typename Out, typename T>
                auto requires_(Out&& o, T&&) -> decltype(
                    concepts::valid_expr(
                        *o = val<T>()
                    ));
            };

            struct IndirectlyMovable
            {
                template<typename I, typename O>
                auto requires_(I&&, O&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable, I>(),
                        concepts::model_of<Movable, Readable::value_t<I>>(),
                        concepts::model_of<Constructible, Readable::value_t<I>,
                            Readable::rvalue_reference_t<I> &&>(),
                        concepts::model_of<Assignable, Readable::value_t<I> &,
                            Readable::rvalue_reference_t<I> &&>(),
                        concepts::model_of<MoveWritable, O, Readable::rvalue_reference_t<I> &&>(),
                        concepts::model_of<MoveWritable, O, Readable::value_t<I> &&>()
                    ));
            };

            struct IndirectlyCopyable
              : refines<IndirectlyMovable>
            {
                template<typename I, typename O>
                auto requires_(I&&, O&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Copyable, Readable::value_t<I>>(),
                        concepts::model_of<Constructible, Readable::value_t<I>,
                            Readable::reference_t<I> &&>(),
                        concepts::model_of<Assignable, Readable::value_t<I> &,
                            Readable::reference_t<I> &&>(),
                        concepts::model_of<Writable, O, Readable::reference_t<I> &&>(),
                        concepts::model_of<Writable, O, Readable::common_reference_t<I> &&>(), // BUGBUG why this?
                        concepts::model_of<Writable, O, Readable::value_t<I> const &>()
                    ));
            };

            struct IndirectlySwappable
            {
                template<typename I1, typename I2>
                auto requires_(I1&& i1, I2&& i2) -> decltype(
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
                using difference_t = meta::_t<difference_type<I>>;

                template<typename I>
                auto requires_(I&& i) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<difference_t<I>>{}),
                        concepts::has_type<I &>(++i),
                        ((void)i++, 42)
                    ));
            };

            struct Incrementable
              : refines<Regular, WeaklyIncrementable>
            {
                template<typename I>
                auto requires_(I&& i) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<I>(i++)
                    ));
            };

            struct WeakIterator
              : refines<WeaklyIncrementable, Copyable>
            {
                template<typename I>
                auto requires_(I&& i) -> decltype(
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
                using category_t = meta::_t<ranges::iterator_category<I>>;

                template<typename I>
                auto requires_(I&& i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::weak_input_iterator_tag>(),
                        concepts::model_of<Readable>(i++)
                    ));
            };

            struct InputIterator
              : refines<WeakInputIterator, Iterator, EqualityComparable>
            {
                template<typename I>
                auto requires_(I&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::input_iterator_tag>()
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator, Incrementable>
            {
                template<typename I>
                auto requires_(I&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::forward_iterator_tag>()
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename I>
                auto requires_(I&& i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::bidirectional_iterator_tag>(),
                        concepts::has_type<I &>(--i),
                        concepts::has_type<I>(i--),
                        concepts::same_type(*i, *i--)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator, TotallyOrdered>
            {
                template<typename I, typename V = common_reference_t<I>>
                auto requires_(I&& i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::random_access_iterator_tag>(),
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
        using iterator_concept_t = meta::_t<iterator_concept<T>>;

        // Generally useful to know if an iterator is single-pass or not:
        template<typename I>
        using SinglePass = meta::fast_and<WeakInputIterator<I>, meta::not_<ForwardIterator<I>>>;

        namespace detail
        {
            // Return the value and reference types of an iterator in a list.
            template<typename I>
            using readable_types_ =
                meta::list<concepts::Readable::value_t<I>, concepts::Readable::reference_t<I>>;

            // Call ApplyFn with the cartesian product of the Readables' value and reference
            // types. In addition, call ApplyFn with the common_reference type of all the
            // Readables. Return all the results as a list.
            template <class...Is>
            using iter_args_lists_ =
                meta::push_back<
                    meta::cartesian_product<
                        meta::transform<meta::list<Is...>, meta::quote<readable_types_>>>,
                    meta::list<concepts::Readable::common_reference_t<Is>...>>;

            template<typename MapFn, typename ReduceFn>
            using iter_map_reduce_fn_ =
                meta::compose<
                    meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
                    meta::quote<iter_args_lists_>>;
        }

        template<typename C, typename ...Is>
        using IndirectFunction = meta::and_<
            meta::fast_and<Readable<Is>...>,
            // C must be callable with the values and references read from the Is.
            meta::lazy::apply<
                detail::iter_map_reduce_fn_<
                    meta::bind_front<meta::quote<Function>, C>,
                    meta::quote<meta::fast_and>>,
                Is...>,
            // In addition, the return types of the C invocations tried above must all
            // share a common reference type. (The lazy::apply is so that this doesn't get
            // evaluated unless C is truly callable as determined above.)
            meta::lazy::apply<
                detail::iter_map_reduce_fn_<
                    meta::bind_front<meta::quote<concepts::Function::result_t>, C>,
                    meta::quote<CommonReference>>,
                Is...> >;

        template<typename C, typename ...Is>
        using IndirectPredicate = meta::and_<
            meta::fast_and<Readable<Is>...>,
            meta::lazy::apply<
                detail::iter_map_reduce_fn_<
                    meta::bind_front<meta::quote<Predicate>, C>,
                    meta::quote<meta::fast_and>>,
                Is...>>;

        template<typename C, typename I0, typename I1 = I0>
        using IndirectRelation = meta::and_<
            meta::fast_and<Readable<I0>, Readable<I1>>,
            meta::lazy::apply<
                detail::iter_map_reduce_fn_<
                    meta::bind_front<meta::quote<Relation>, C>,
                    meta::quote<meta::fast_and>>,
                I0, I1>>;

        template<typename C, typename ...Is>
        using IndirectCallable = IndirectFunction<function_type<C>, Is...>;

        template<typename C, typename ...Is>
        using IndirectCallablePredicate = IndirectPredicate<function_type<C>, Is...>;

        template<typename C, typename I0, typename I1 = I0>
        using IndirectCallableRelation = IndirectRelation<function_type<C>, I0, I1>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Project struct, for "projecting" a Readable with a unary callable
        namespace detail
        {
            template<typename I, typename Proj>
            struct projected_readable
            {
                using value_type =
                    decay_t<concepts::Callable::result_t<Proj, concepts::Readable::value_t<I>>>;
                using reference =
                    concepts::Callable::result_t<Proj, concepts::Readable::reference_t<I>>;
                reference operator*() const;
            };
        }

        template<typename I, typename Proj>
        using Projected = meta::if_<IndirectCallable<Proj, I>, detail::projected_readable<I, Proj>>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Composite concepts for use defining algorithms:
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
            IndirectCallableRelation<C, Projected<I0, P0>, Projected<I1, P1>>,
            IndirectlyCopyable<I0, Out>,
            IndirectlyCopyable<I1, Out>>;

        template<typename I0, typename I1, typename Out, typename C = ordered_less,
            typename P0 = ident, typename P1 = ident>
        using MergeMovable = meta::fast_and<
            InputIterator<I0>,
            InputIterator<I1>,
            WeaklyIncrementable<Out>,
            IndirectCallableRelation<C, Projected<I0, P0>, Projected<I1, P1>>,
            IndirectlyMovable<I0, Out>,
            IndirectlyMovable<I1, Out>>;

        template<typename I, typename C = ordered_less, typename P = ident>
        using Sortable = meta::fast_and<
            ForwardIterator<I>,
            IndirectCallableRelation<C, Projected<I, P>, Projected<I, P>>,
            Permutable<I>>;

        template<typename I, typename V2, typename C = ordered_less, typename P = ident>
        using BinarySearchable = meta::fast_and<
            ForwardIterator<I>,
            IndirectCallableRelation<C, Projected<I, P>, V2 const *>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using WeaklyAsymmetricallyComparable = meta::fast_and<
            InputIterator<I1>,
            WeakInputIterator<I2>,
            IndirectCallablePredicate<C, Projected<I1, P1>, Projected<I2, P2>>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using AsymmetricallyComparable = meta::fast_and<
            WeaklyAsymmetricallyComparable<I1, I2, C, P1, P2>,
            InputIterator<I2>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using WeaklyComparable = meta::fast_and<
            WeaklyAsymmetricallyComparable<I1, I2, C, P1, P2>,
            IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>>;

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
                auto requires_(I&& i, S&& s) -> decltype(
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
                auto requires_(I&& i, S&& s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(s - i),
                        concepts::same_type(s - i, i - s)
                    ));
            };

            struct SizedIteratorRange
              : refines<IteratorRange>
            {
                template<typename I, typename S,
                    meta::if_<std::is_same<I, S>, int> = 0>
                auto requires_(I&& i, I&& s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(s - i)
                    ));

                template<typename I, typename S,
                    meta::if_c<!std::is_same<I, S>::value, int> = 0,
                    typename C = common_type_t<I, S>>
                auto requires_(I&& i, S&& s) -> decltype(
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
        using sized_iterator_range_concept_t = meta::_t<sized_iterator_range_concept<I, S>>;
        /// @}
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
