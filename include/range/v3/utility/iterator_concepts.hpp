/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
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
        struct input_iterator_tag
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

        struct contiguous_iterator_tag
          : random_access_iterator_tag
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

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T>
            meta::if_<std::is_object<T>, ranges::contiguous_iterator_tag>
            iterator_category_helper(T **);

            template<typename T>
            meta::_t<upgrade_iterator_category<typename T::iterator_category>>
            iterator_category_helper(T *);

            template<class T>
            using iterator_category_ = decltype(detail::iterator_category_helper(_nullptr_v<T>()));
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        template<typename T>
        struct iterator_category
          : meta::defer<detail::iterator_category_, T>
        {};

        template<typename T>
        struct iterator_category<T const>
          : iterator_category<T>
        {};

        namespace concepts
        {
            struct Readable
            {
            private:
                template<typename I,
                    typename = reference_t<I>,
                    typename R = decltype(iter_move(std::declval<I &>())),
                    typename = R&>
                using rvalue_reference_t_ = R;
            public:
                // Associated types
                template<typename I>
                using value_t = meta::_t<value_type<I>>;

                template<typename I>
                using rvalue_reference_t = rvalue_reference_t_<I>;

                template<typename I>
                using common_reference_t =
                    ranges::common_reference_t<reference_t<I>, value_t<I> &>;

                template<typename I>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        // The value, reference and rvalue reference types are related
                        // through the CommonReference concept.
                        concepts::model_of<CommonReference, reference_t<I> &&, value_t<I> &>(),
                        concepts::model_of<CommonReference, reference_t<I> &&, rvalue_reference_t<I> &&>(),
                        concepts::model_of<CommonReference, rvalue_reference_t<I> &&, value_t<I> const &>()
                    ));
            };

            struct Writable
            {
                template<typename Out, typename T>
                auto requires_(Out &&o, T &&t) -> decltype(
                    concepts::valid_expr(
                        ((void)(*o = static_cast<T &&>(t)), 42),
                        ((void)(*static_cast<Out &&>(o) = static_cast<T &&>(t)), 42),
                        ((void)(const_cast<reference_t<Out> const &&>(*o) = static_cast<T &&>(t)), 42),
                        ((void)(const_cast<reference_t<Out> const &&>(*static_cast<Out &&>(o)) = static_cast<T &&>(t)), 42)
                    ));
            };

            struct IndirectlyMovable
            {
                template<typename I, typename O>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable, I>(),
                        concepts::model_of<Writable, O, Readable::rvalue_reference_t<I>>()
                    ));
            };

            struct IndirectlyMovableStorable
              : refines<IndirectlyMovable>
            {
                template<typename I, typename O>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Movable, Readable::value_t<I>>(),
                        concepts::model_of<Constructible, Readable::value_t<I>,
                            Readable::rvalue_reference_t<I>>(),
                        concepts::model_of<Assignable, Readable::value_t<I> &,
                            Readable::rvalue_reference_t<I>>(),
                        concepts::model_of<Writable, O, Readable::value_t<I>>()
                    ));
            };

            struct IndirectlyCopyable
            {
                template<typename I, typename O>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Readable, I>(),
                        concepts::model_of<Writable, O, reference_t<I>>()
                    ));
            };

            struct IndirectlyCopyableStorable
              : refines<IndirectlyCopyable>
            {
                template<typename I, typename O>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Copyable, Readable::value_t<I>>(),
                        concepts::model_of<Constructible, Readable::value_t<I>, reference_t<I>>(),
                        concepts::model_of<Assignable, Readable::value_t<I> &, reference_t<I>>(),
                        concepts::model_of<Writable, O, Readable::common_reference_t<I>>(),
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
                        ((void)ranges::iter_swap((I1 &&) i1, (I2 &&) i2), 42),
                        ((void)ranges::iter_swap((I1 &&) i1, (I1 &&) i1), 42),
                        ((void)ranges::iter_swap((I2 &&) i2, (I2 &&) i2), 42),
                        ((void)ranges::iter_swap((I2 &&) i2, (I1 &&) i1), 42)
                    ));
            };

            struct WeaklyIncrementable
              : refines<SemiRegular>
            {
                // Associated types
                template<typename I>
                using difference_t = meta::_t<difference_type<I>>;

                template<typename I>
                auto requires_(I i) -> decltype(
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
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<I>(i++)
                    ));
            };

            struct Iterator
              : refines<WeaklyIncrementable, Copyable>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        ((void)*i, 42)
                    ));
            };

            struct Sentinel
              : refines<SemiRegular(_1), Iterator(_2), WeaklyEqualityComparable>
            {};

            struct SizedSentinel
              : refines<Sentinel>
            {
                template<typename S, typename I,
                    typename D = WeaklyIncrementable::difference_t<I>>
                auto requires_(S s, I i) -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(
                            disable_sized_sentinel<uncvref_t<S>, uncvref_t<I>>()),
                        concepts::has_type<D>(s - i),
                        concepts::has_type<D>(i - s)
                    ));
            };

            struct OutputIterator
              : refines<Iterator(_1), Writable>
            {
                template<typename Out, typename T>
                auto requires_(Out o, T &&t) -> decltype(
                    concepts::valid_expr(
                        ((void)(*o++ = (T &&) t), 42)
                    ));
            };

            struct InputIterator
              : refines<Iterator, Readable>
            {
                // Associated types
                // value_t from Readable
                // distance_t from WeaklyIncrementable
                template<typename I>
                using category_t = meta::_t<ranges::iterator_category<I>>;

                template<typename I>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::input_iterator_tag>()
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator, Incrementable, Sentinel(_1, _1)>
            {
                template<typename I>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::forward_iterator_tag>()
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::bidirectional_iterator_tag>(),
                        concepts::has_type<I &>(--i),
                        concepts::has_type<I>(i--),
                        concepts::same_type(*i, *i--)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator, TotallyOrdered, SizedSentinel(_1, _1)>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::random_access_iterator_tag>(),
                        concepts::has_type<I>(i + (i - i)),
                        concepts::has_type<I>((i - i) + i),
                        concepts::has_type<I>(i - (i - i)),
                        concepts::has_type<I &>(i += (i-i)),
                        concepts::has_type<I &>(i -= (i - i)),
                        concepts::model_of<Same, reference_t<I>, decltype(i[i - i])>()
                    ));
            };

            struct ContiguousIterator
              : refines<RandomAccessIterator>
            {
                template<typename I>
                auto requires_(I i) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DerivedFrom, category_t<I>, ranges::contiguous_iterator_tag>(),
                        concepts::is_true(std::is_lvalue_reference<reference_t<I>>{}),
                        concepts::model_of<Same, value_t<I>, uncvref_t<reference_t<I>>>()
                    ));
            };
        }

        template<typename T>
        using Readable = concepts::models<concepts::Readable, T>;

        template<typename Out, typename T>
        using Writable = concepts::models<concepts::Writable, Out, T>;

        template<typename I, typename O>
        using IndirectlyMovable = concepts::models<concepts::IndirectlyMovable, I, O>;

        template<typename I, typename O>
        using IndirectlyMovableStorable = concepts::models<concepts::IndirectlyMovableStorable, I, O>;

        template<typename I, typename O>
        using IndirectlyCopyable = concepts::models<concepts::IndirectlyCopyable, I, O>;

        template<typename I, typename O>
        using IndirectlyCopyableStorable = concepts::models<concepts::IndirectlyCopyableStorable, I, O>;

        template<typename I1, typename I2>
        using IndirectlySwappable = concepts::models<concepts::IndirectlySwappable, I1, I2>;

        template<typename T>
        using WeaklyIncrementable = concepts::models<concepts::WeaklyIncrementable, T>;

        template<typename T>
        using Incrementable = concepts::models<concepts::Incrementable, T>;

        template<typename I>
        using Iterator = concepts::models<concepts::Iterator, I>;

        template<typename S, typename I>
        using Sentinel = concepts::models<concepts::Sentinel, S, I>;

        template<typename S, typename I>
        using SizedSentinel = concepts::models<concepts::SizedSentinel, S, I>;

        template<typename I, typename S>
        using IteratorRange RANGES_DEPRECATED("Please use Sentinel<S, I>() instead") =
            Sentinel<S, I>;

        template<typename I, typename S>
        using SizedIteratorRange RANGES_DEPRECATED("Please use SizedSentinel<S, I>() instead") =
            SizedSentinel<S, I>;

        template<typename Out, typename T>
        using OutputIterator = concepts::models<concepts::OutputIterator, Out, T>;

        template<typename I>
        using InputIterator = concepts::models<concepts::InputIterator, I>;

        template<typename I>
        using ForwardIterator = concepts::models<concepts::ForwardIterator, I>;

        template<typename I>
        using BidirectionalIterator = concepts::models<concepts::BidirectionalIterator, I>;

        template<typename I>
        using RandomAccessIterator = concepts::models<concepts::RandomAccessIterator, I>;

        template<typename I>
        using ContiguousIterator = concepts::models<concepts::ContiguousIterator, I>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator_concept
        template<typename T>
        using iterator_concept =
            concepts::most_refined<
                meta::list<
                    concepts::ContiguousIterator,
                    concepts::RandomAccessIterator,
                    concepts::BidirectionalIterator,
                    concepts::ForwardIterator,
                    concepts::InputIterator>, T>;

        template<typename T>
        using iterator_concept_t = meta::_t<iterator_concept<T>>;

        // Generally useful to know if an iterator is single-pass or not:
        template<typename I>
        using SinglePass = meta::strict_and<Iterator<I>, meta::not_<ForwardIterator<I>>>;

        /// \cond
        namespace detail
        {
            template<typename I, bool IsReadable = (bool) Readable<I>()>
            struct exclusively_writable_
            {
                template<typename T>
                using invoke = Writable<I, T>;
            };

            template<typename I>
            struct exclusively_writable_<I, true>
            {
                template<typename T, typename U>
                using assignable_res_t = decltype(std::declval<T>() = std::declval<U>());

                template<typename T>
                using invoke =
                    meta::and_<
                        Writable<I, T>,
                        meta::not_<meta::is_trait<meta::defer<assignable_res_t, reference_t<I>, T>>>>;
            };
        }
        /// \endcond

        template<typename I, typename T>
        using ExclusivelyWritable_ = meta::invoke<detail::exclusively_writable_<I>, T>;

        namespace detail
        {
            // Return the value and reference types of an iterator in a list.
            template<typename I>
            using readable_types_ =
                meta::list<concepts::Readable::value_t<I> &, reference_t<I> /*&&*/>;

            // Call ApplyFn with the cartesian product of the Readables' value and reference
            // types. In addition, call ApplyFn with the common_reference type of all the
            // Readables. Return all the results as a list.
            template<class...Is>
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

            template<template <typename...> class InvocableConcept, typename C, typename ...Is>
            using indirect_invocable_ = meta::and_<
                meta::strict_and<Readable<Is>...>,
                // C must satisfy the InvocableConcept with the values and references read from the Is.
                meta::lazy::invoke<
                    iter_map_reduce_fn_<
                        meta::bind_front<meta::quote<InvocableConcept>, C&>,
                        meta::quote<meta::strict_and>>,
                    Is...>>;

            template<typename C, typename ...Is>
            using common_result_indirect_invocable_ = meta::and_<
                indirect_invocable_<Invocable, C, Is...>,
                // In addition to C being invocable, the return types of the C invocations must all
                // share a common reference type. (The lazy::invoke is so that this doesn't get
                // evaluated unless C is truly callable as determined above.)
                meta::lazy::invoke<
                    iter_map_reduce_fn_<
                        meta::bind_front<meta::quote<concepts::Invocable::result_t>, C&>,
                        meta::quote<CommonReference>>,
                    Is...>>;
        }

        template<typename C, typename ...Is>
        using IndirectInvocable = meta::and_<
            detail::common_result_indirect_invocable_<C, Is...>,
            CopyConstructible<C>>;

        template<typename C, typename ...Is>
        using MoveIndirectInvocable = meta::and_<
            detail::common_result_indirect_invocable_<C, Is...>,
            MoveConstructible<C>>;

        template<typename C, typename ...Is>
        using IndirectRegularInvocable = IndirectInvocable<C, Is...>;

        template<typename C, typename ...Is>
        using IndirectPredicate = meta::and_<
            detail::indirect_invocable_<Predicate, C, Is...>,
            CopyConstructible<C>>;

        template<typename C, typename I0, typename I1 = I0>
        using IndirectRelation = meta::and_<
            detail::indirect_invocable_<Relation, C, I0, I1>,
            CopyConstructible<C>>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // indirect_invoke_result
        /// \cond
        template<typename Fun, typename... Is>
        using indirect_invoke_result_t =
            meta::if_c<
                meta::and_c<(bool) Readable<Is>()...>::value,
                invoke_result_t<Fun, reference_t<Is>...>>;

        template<typename Fun, typename... Is>
        struct indirect_invoke_result
          : meta::defer<indirect_invoke_result_t, Fun, Is...>
        {};

        ////////////////////////////////////////////////////////////////////////////////////////////
        // indirect_result_of
        template<typename Sig>
        struct indirect_result_of
        {};

        template<typename Fun, typename... Is>
        struct indirect_result_of<Fun(Is...)>
          : meta::defer<indirect_invoke_result_t, Fun, Is...>
        {};

        template<typename Sig>
        using indirect_result_of_t = meta::_t<indirect_result_of<Sig>>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Project struct, for "projecting" a Readable with a unary callable
        /// \cond
        namespace detail
        {
            template<typename I, typename Proj>
            struct projected_
            {
                using reference = indirect_invoke_result_t<Proj &, I>;
                using value_type = uncvref_t<reference>;
                reference operator*() const;
            };
        }
        /// \endcond

        template<typename I, typename Proj>
        using projected =
            meta::if_c<
                IndirectRegularInvocable<Proj, I>::value,
                detail::projected_<I, Proj>>;

        template<typename I, typename Proj>
        struct difference_type<detail::projected_<I, Proj>>
        {
            using type = meta::_t<difference_type<I>>;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Composite concepts for use defining algorithms:
        template<typename I, typename V = concepts::Readable::value_t<I>>
        using Permutable = meta::strict_and<
            ForwardIterator<I>,
            IndirectlySwappable<I, I>,
            IndirectlyMovableStorable<I, I>>;

        template<typename I0, typename I1, typename Out, typename C = ordered_less,
            typename P0 = ident, typename P1 = ident>
        using Mergeable = meta::strict_and<
            InputIterator<I0>,
            InputIterator<I1>,
            WeaklyIncrementable<Out>,
            IndirectRelation<C, projected<I0, P0>, projected<I1, P1>>,
            IndirectlyCopyable<I0, Out>,
            IndirectlyCopyable<I1, Out>>;

        template<typename I, typename C = ordered_less, typename P = ident>
        using Sortable = meta::strict_and<
            ForwardIterator<I>,
            IndirectRelation<C, projected<I, P>, projected<I, P>>,
            Permutable<I>>;

        template<typename I, typename V2, typename C = ordered_less, typename P = ident>
        using BinarySearchable = meta::strict_and<
            ForwardIterator<I>,
            IndirectRelation<C, projected<I, P>, V2 const *>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using AsymmetricallyComparable = meta::strict_and<
            InputIterator<I1>,
            InputIterator<I2>,
            IndirectPredicate<C, projected<I1, P1>, projected<I2, P2>>>;

        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Comparable = meta::strict_and<
            AsymmetricallyComparable<I1, I2, C, P1, P2>,
            IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>>;

        template<typename S, typename I>
        using sized_sentinel_concept =
            concepts::most_refined<
                meta::list<
                    concepts::SizedSentinel,
                    concepts::Sentinel>, S, I>;

        template<typename S, typename I>
        using sized_sentinel_concept_t = meta::_t<sized_sentinel_concept<S, I>>;
        /// @}
    }
}


#ifdef _GLIBCXX_DEBUG
// HACKHACK: workaround underconstrained operator- for libstdc++ debug iterator wrapper
// by intentionally creating an ambiguity when the wrapped types don't support the
// necessary operation.
#include <debug/safe_iterator.h>

namespace __gnu_debug
{
    template<class I1, class I2, class Seq,
        CONCEPT_REQUIRES_(!::ranges::SizedSentinel<I1, I2>())>
    void operator-(
        _Safe_iterator<I1, Seq> const &, _Safe_iterator<I2, Seq> const &) = delete;

    template<class I1, class Seq,
        CONCEPT_REQUIRES_(!::ranges::SizedSentinel<I1, I1>())>
    void operator-(
        _Safe_iterator<I1, Seq> const &, _Safe_iterator<I1, Seq> const &) = delete;
}
#endif

#if defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)
// HACKHACK: workaround libc++ (https://llvm.org/bugs/show_bug.cgi?id=28421)
// and libstdc++ (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71771)
// underconstrained operator- for reverse_iterator by disabling SizedSentinel
// when the base iterators do not model SizedSentinel.
namespace ranges
{
    inline namespace v3
    {
        template<typename S, typename I>
        struct disable_sized_sentinel<std::reverse_iterator<S>, std::reverse_iterator<I>>
          : meta::not_<SizedSentinel<I, S>>
        {};
    }
}
#endif // defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
