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
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/nullptr_v.hpp>

#ifdef _GLIBCXX_DEBUG
#include <debug/safe_iterator.h>
#endif

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I>
            using iter_traits_t =
                if_then_t<is_std_iterator_traits_specialized_<I>, std::iterator_traits<I>, I>;

#if defined(_GLIBCXX_DEBUG)
            template<typename T, typename Seq>
            auto iter_concept_(__gnu_debug::_Safe_iterator<T *, Seq>, int) -> ranges::contiguous_iterator_tag;
#endif
#if defined(__GLIBCXX__)
            template<typename T, typename Seq>
            auto iter_concept_(__gnu_cxx::__normal_iterator<T *, Seq>, int) -> ranges::contiguous_iterator_tag;
#endif
#if defined(_LIBCPP_VERSION)
            template<typename T>
            auto iter_concept_(std::__wrap_iter<T *>, int) -> ranges::contiguous_iterator_tag;
#endif
            template<typename T>
            auto iter_concept_(T *, int) -> ranges::contiguous_iterator_tag;
            template<typename I>
            auto iter_concept_(I, int) -> typename iter_traits_t<I>::iterator_concept;
            template<typename I>
            auto iter_concept_(I, long) -> typename iter_traits_t<I>::iterator_category;
            template<typename I>
            auto iter_concept_(I, ...) ->
                enable_if_t<
                    !is_std_iterator_traits_specialized_<I>,
                    ranges::random_access_iterator_tag>;

            template<typename I>
            using iter_concept_t = decltype(iter_concept_(std::declval<I>(), 0));
        }

        /// \endcond
        CPP_def
        (
            template(typename I)
            concept Readable,
                CommonReference<iter_reference_t<I> &&, iter_value_t<I> &> &&
                CommonReference<iter_reference_t<I> &&, iter_rvalue_reference_t<I> &&> &&
                CommonReference<iter_rvalue_reference_t<I> &&, iter_value_t<I> const &>
        );

        CPP_def
        (
            template(typename Out, typename T)
            concept Writable,
                requires (Out &&o, T &&t)
                (
                    *o = static_cast<T &&>(t),
                    *((Out &&) o) = static_cast<T &&>(t),
                    const_cast<iter_reference_t<Out> const &&>(*o) = static_cast<T &&>(t),
                    const_cast<iter_reference_t<Out> const &&>(*((Out &&) o)) = static_cast<T &&>(t)
                )
        );

        CPP_def
        (
            template(typename I, typename O)
            concept IndirectlyMovable,
                Readable<I> && Writable<O, iter_rvalue_reference_t<I>>
        );

        CPP_def
        (
            template(typename I, typename O)
            concept IndirectlyMovableStorable,
                IndirectlyMovable<I, O> && 
                Movable<iter_value_t<I>> &&
                Constructible<iter_value_t<I>, iter_rvalue_reference_t<I>> &&
                Assignable<iter_value_t<I> &, iter_rvalue_reference_t<I>> &&
                Writable<O, iter_value_t<I>>
        );

        CPP_def
        (
            template(typename I, typename O)
            concept IndirectlyCopyable,
                Readable<I> && Writable<O, iter_reference_t<I>>
        );

        CPP_def
        (
            template(typename I, typename O)
            concept IndirectlyCopyableStorable,
                IndirectlyMovable<I, O> && 
                Copyable<iter_value_t<I>> &&
                Constructible<iter_value_t<I>, iter_reference_t<I>> &&
                Assignable<iter_value_t<I> &, iter_reference_t<I>> &&
                Writable<O, iter_common_reference_t<I>> &&
                Writable<O, iter_value_t<I> const &>
        );

        CPP_def
        (
            template(typename I1, typename I2)
            concept IndirectlySwappable,
                requires (I1 && i1, I2 && i2)
                (
                    ranges::iter_swap((I1 &&) i1, (I2 &&) i2),
                    ranges::iter_swap((I1 &&) i1, (I1 &&) i1),
                    ranges::iter_swap((I2 &&) i2, (I2 &&) i2),
                    ranges::iter_swap((I2 &&) i2, (I1 &&) i1)
                ) &&
                Readable<I1> && Readable<I2>
        );

        CPP_def
        (
            template(typename I)
            concept WeaklyIncrementable,
                requires (I i)
                (
                    Type<iter_difference_t<I>>,
                    ++i,
                    i++,
                    concepts::requires_<Same<I&, decltype(++i)>>
                ) &&
                Integral<iter_difference_t<I>> &&
                Semiregular<I>
        );

        CPP_def
        (
            template(typename I)
            concept Incrementable,
                requires (I i)
                (
                    concepts::requires_<Same<I, decltype(i++)>>
                ) &&
                Regular<I> && WeaklyIncrementable<I>
        );

        CPP_def
        (
            template(typename I)
            concept Iterator,
                requires (I i)
                (
                    *i
                ) &&
                WeaklyIncrementable<I>
        );

        CPP_def
        (
            template(typename S, typename I)
            concept Sentinel,
                Semiregular<S> && Iterator<I> &&
                WeaklyEqualityComparableWith<S, I>
        );

        CPP_def
        (
            template(typename S, typename I)
            concept SizedSentinel,
                requires (S const &s, I const &i)
                (
                    s - i,
                    i - s,
                    concepts::requires_<Same<iter_difference_t<I>, decltype(s - i)>>,
                    concepts::requires_<Same<iter_difference_t<I>, decltype(i - s)>>
                ) &&
                // Short-circuit the test for Sentinel if we're emulating concepts:
                (!defer::Satisfies<uncvref_t<S>, disable_sized_sentinel, uncvref_t<I>> &&
                defer::Sentinel<S, I>)
        );

        CPP_def
        (
            template(typename Out, typename T)
            concept OutputIterator,
                requires (Out o, T &&t)
                (
                    *o++ = static_cast<T &&>(t)
                ) &&
                Iterator<Out> && Writable<Out, T>
        );

        CPP_def
        (
            template(typename I)
            concept InputIterator,
                Iterator<I> && Readable<I> &&
                DerivedFrom<detail::iter_concept_t<I>, ranges::input_iterator_tag>
        );

        CPP_def
        (
            template(typename I)
            concept ForwardIterator,
                InputIterator<I> && Incrementable<I> &&
                Sentinel<I, I> &&
                DerivedFrom<detail::iter_concept_t<I>, ranges::forward_iterator_tag>
        );

        CPP_def
        (
            template(typename I)
            concept BidirectionalIterator,
                requires (I i)
                (
                    --i,
                    i--,
                    concepts::requires_<Same<I&, decltype(--i)>>,
                    concepts::requires_<Same<I, decltype(i--)>>
                ) &&
                ForwardIterator<I> &&
                DerivedFrom<detail::iter_concept_t<I>, ranges::bidirectional_iterator_tag>
        );

        CPP_def
        (
            template(typename I)
            concept RandomAccessIterator,
                requires (I i, iter_difference_t<I> n)
                (
                    i + n,
                    n + i,
                    i - n,
                    i += n,
                    i -= n,
                    concepts::requires_<Same<decltype(i + n), I>>,
                    concepts::requires_<Same<decltype(n + i), I>>,
                    concepts::requires_<Same<decltype(i - n), I>>,
                    concepts::requires_<Same<decltype(i += n), I&>>,
                    concepts::requires_<Same<decltype(i -= n), I&>>,
                    concepts::requires_<Same<decltype(i[n]), iter_reference_t<I>>>
                ) &&
                BidirectionalIterator<I> &&
                StrictTotallyOrdered<I> &&
                SizedSentinel<I, I> &&
                DerivedFrom<detail::iter_concept_t<I>, ranges::random_access_iterator_tag>
        );

        CPP_def
        (
            template(typename I)
            concept ContiguousIterator,
                RandomAccessIterator<I> &&
                DerivedFrom<detail::iter_concept_t<I>, ranges::contiguous_iterator_tag> &&
                std::is_lvalue_reference<iter_reference_t<I>>::value &&
                Same<iter_value_t<I>, uncvref_t<iter_reference_t<I>>>
        );

        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator_tag_of
        template<typename T>
        using iterator_tag_of =
            concepts::tag_of<
                meta::list<
                    ContiguousIteratorConcept,
                    RandomAccessIteratorConcept,
                    BidirectionalIteratorConcept,
                    ForwardIteratorConcept,
                    InputIteratorConcept>,
                T>;

        /// \cond
        namespace detail
        {
            template<typename, bool>
            struct iterator_category_
            {};

            template<typename I>
            struct iterator_category_<I, true>
            {
            private:
                static ranges::input_iterator_tag test(detail::input_iterator_tag);
                static ranges::forward_iterator_tag test(detail::forward_iterator_tag);
                static ranges::bidirectional_iterator_tag test(detail::bidirectional_iterator_tag);
                static ranges::random_access_iterator_tag test(detail::random_access_iterator_tag);
                static ranges::contiguous_iterator_tag test(detail::contiguous_iterator_tag);
            public:
                using type = decltype(iterator_category_::test(iterator_tag_of<I>{}));
            };

            template<typename T>
            using iterator_category =
                iterator_category_<
                    meta::_t<std::remove_const<T>>,
                    (bool) InputIterator<meta::_t<std::remove_const<T>>>>;
        }
        /// \endcond

        /// \cond
        template<typename I>
        using iterator_category
            RANGES_DEPRECATED("iterator_category is deprecated. Use the iterator concepts instead") =
                detail::iterator_category<I>;

        template<typename I>
        using iterator_category_t
            RANGES_DEPRECATED("iterator_category_t is deprecated. Use the iterator concepts instead") =
                meta::_t<detail::iterator_category<I>>;
        /// \endcond

        // Generally useful to know if an iterator is single-pass or not:
        CPP_def
        (
            template(typename I)
            concept SinglePass,
                Iterator<I> && !ForwardIterator<I>
        );

        /// \cond
        namespace detail
        {
            template<typename I, bool IsReadable>
            struct exclusively_writable_
            {
                template<typename T>
                using invoke = meta::bool_<(bool) Writable<I, T>>;
            };

            template<typename I>
            struct exclusively_writable_<I, true>
            {
                template<typename T, typename U>
                using assignable_res_t = decltype(std::declval<T>() = std::declval<U>());

                template<typename T>
                using invoke =
                    meta::and_<
                        meta::bool_<(bool) Writable<I, T>>,
                        meta::not_<meta::is_trait<meta::defer<assignable_res_t, iter_reference_t<I>, T>>>>;
            };

            template<typename I>
            using exclusively_writable = exclusively_writable_<I, (bool) Readable<I>>;
        }
        /// \endcond

        template<typename I, typename T>
        using ExclusivelyWritable_ = meta::invoke<detail::exclusively_writable<I>, T>;

        /// \cond
        namespace detail
        {
            template<typename T, typename U>
            using variadic_common_reference2_ =
                meta::if_c<(bool)CommonReference<T, U>, common_reference_t<T, U>>;

            template<typename... Ts>
            struct variadic_common_reference_
              : std::true_type
            {};

            template<typename T, typename... Rest>
            struct variadic_common_reference_<T, Rest...>
              : meta::is_trait<
                    meta::lazy::fold<meta::list<Rest...>, T, meta::quote<variadic_common_reference2_>>>
            {};

            // Return the value and reference types of an iterator in a list.
            template<typename I>
            using readable_types_ =
                meta::list<iter_value_t<I> &, iter_reference_t<I> /*&&*/>;

            // Call ApplyFn with the cartesian product of the Readables' value and reference
            // types. In addition, call ApplyFn with the common_reference type of all the
            // Readables. Return all the results as a list.
            template<typename...Is>
            using iter_args_lists_ =
                meta::push_back<
                    meta::cartesian_product<
                        meta::transform<meta::list<Is...>, meta::quote<readable_types_>>>,
                    meta::list<iter_common_reference_t<Is>...>>;

            template<typename MapFn, typename ReduceFn>
            using iter_map_reduce_fn_ =
                meta::compose<
                    meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
                    meta::quote<iter_args_lists_>>;

            template<typename InvocableConcept, typename C, typename ...Is>
            using indirect_invocable_ = meta::and_<
                meta::and_c<Readable<Is>...>,
                // C must satisfy the InvocableConcept with the values and references read from the Is.
                meta::lazy::invoke<
                    iter_map_reduce_fn_<
                        meta::bind_front<InvocableConcept, C&>,
                        meta::quote<meta::strict_and>>,
                    Is...>>;

            template<typename C, typename ...Is>
            using common_result_indirect_invocable_ = meta::and_<
                indirect_invocable_<
                    meta::bind_front<meta::quote<concepts::is_satisfied_by>, InvocableConcept>, C, Is...>,
                // In addition to C being invocable, the return types of the C invocations must all
                // share a common reference type. (The lazy::invoke is so that this doesn't get
                // evaluated unless C is truly callable as determined above.)
                meta::lazy::invoke<
                    iter_map_reduce_fn_<
                        meta::bind_front<meta::quote<invoke_result_t>, C&>,
                        meta::quote<variadic_common_reference_>>,
                    Is...>>;
        }
        /// \endcond

        CPP_def
        (
            template(typename C, typename ...Is)
            (concept IndirectInvocable)(C, Is...),
                detail::common_result_indirect_invocable_<C, Is...>::value &&
                CopyConstructible<C>
        );

        CPP_def
        (
            template(typename C, typename ...Is)
            (concept MoveIndirectInvocable)(C, Is...),
                detail::common_result_indirect_invocable_<C, Is...>::value &&
                MoveConstructible<C>
        );

        CPP_def
        (
            template(typename C, typename ...Is)
            (concept IndirectRegularInvocable)(C, Is...),
                IndirectInvocable<C, Is...>
        );

        CPP_def
        (
            template(typename C, typename ...Is)
            (concept IndirectPredicate)(C, Is...),
                detail::indirect_invocable_<
                    meta::bind_front<meta::quote<concepts::is_satisfied_by>, PredicateConcept>, C, Is...>::value &&
                CopyConstructible<C>
        );

        CPP_def
        (
            template(typename C, typename I0, typename I1 = I0)
            (concept IndirectRelation)(C, I0, I1),
                detail::indirect_invocable_<
                    meta::bind_front<meta::quote<concepts::is_satisfied_by>, RelationConcept>, C, I0, I1>::value &&
                CopyConstructible<C>
        );

        ////////////////////////////////////////////////////////////////////////////////////////////
        // indirect_result_t
        template<typename Fun, typename... Is>
        using indirect_result_t =
            meta::if_c<
                meta::and_c<(bool) Readable<Is>...>::value,
                invoke_result_t<Fun, iter_reference_t<Is>...>>;

        /// \cond
        template<typename Fun, typename... Is>
        using indirect_invoke_result_t RANGES_DEPRECATED("Please switch to indirect_result_t") =
            indirect_result_t<Fun, Is...>;

        template<typename Fun, typename... Is>
        struct RANGES_DEPRECATED("Please switch to indirect_result_t") indirect_invoke_result
          : meta::defer<indirect_invoke_result_t, Fun, Is...>
        {};

        template<typename Sig>
        struct indirect_result_of
        {};

        template<typename Fun, typename... Is>
        struct RANGES_DEPRECATED("Please switch to indirect_result_t") indirect_result_of<Fun(Is...)>
          : meta::defer<indirect_result_t, Fun, Is...>
        {};

        template<typename Sig>
        using indirect_result_of_t RANGES_DEPRECATED("Please switch to indirect_result_t") =
            meta::_t<indirect_result_of<Sig>>;
        /// \endcond

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Project struct, for "projecting" a Readable with a unary callable
        /// \cond
        namespace detail
        {
            template<typename I, typename Proj>
            struct projected_
            {
                using reference = indirect_result_t<Proj &, I>;
                using value_type = uncvref_t<reference>;
                reference operator*() const;
            };
        }
        /// \endcond

        template<typename I, typename Proj>
        using projected =
            meta::if_c<
                (bool)IndirectRegularInvocable<Proj, I>,
                detail::projected_<I, Proj>>;

        template<typename I, typename Proj>
        struct incrementable_traits<detail::projected_<I, Proj>>
          : incrementable_traits<I>
        {};

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Composite concepts for use defining algorithms:
        CPP_def
        (
            template(typename I)
            concept Permutable,
                ForwardIterator<I> &&
                IndirectlySwappable<I, I> &&
                IndirectlyMovableStorable<I, I>
        );

        CPP_def
        (
            template(typename I0, typename I1, typename Out, typename C = ordered_less,
                typename P0 = ident, typename P1 = ident)
            (concept Mergeable)(I0, I1, Out, C, P0, P1),
                InputIterator<I0> &&
                InputIterator<I1> &&
                WeaklyIncrementable<Out> &&
                IndirectRelation<C, projected<I0, P0>, projected<I1, P1>> &&
                IndirectlyCopyable<I0, Out> &&
                IndirectlyCopyable<I1, Out>
        );

        CPP_def
        (
            template(typename I0, typename I1, typename Out, typename C = ordered_less,
                typename P0 = ident, typename P1 = ident)
            (concept MoveMergeable)(I0, I1, Out, C, P0, P1),
                InputIterator<I0> &&
                InputIterator<I1> &&
                WeaklyIncrementable<Out> &&
                IndirectRelation<C, projected<I0, P0>, projected<I1, P1>> &&
                IndirectlyMovable<I0, Out> &&
                IndirectlyMovable<I1, Out>
        );

        CPP_def
        (
            template(typename I, typename C = ordered_less, typename P = ident)
            (concept Sortable)(I, C, P),
                ForwardIterator<I> &&
                IndirectRelation<C, projected<I, P>, projected<I, P>> &&
                Permutable<I>
        );

        CPP_def
        (
            template(typename I, typename V2, typename C = ordered_less, typename P = ident)
            (concept BinarySearchable)(I, V2, C, P),
                ForwardIterator<I> &&
                IndirectRelation<C, projected<I, P>, V2 const *>
        );

        CPP_def
        (
            template(typename I1, typename I2, typename C = equal_to, typename P1 = ident,
                typename P2 = ident)
            (concept AsymmetricallyComparable)(I1, I2, C, P1, P2),
                InputIterator<I1> &&
                InputIterator<I2> &&
                IndirectPredicate<C, projected<I1, P1>, projected<I2, P2>>
        );

        CPP_def
        (
            template(typename I1, typename I2, typename C = equal_to, typename P1 = ident,
                typename P2 = ident)
            (concept Comparable)(I1, I2, C, P1, P2),
                AsymmetricallyComparable<I1, I2, C, P1, P2> &&
                IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>
        );

        using sentinel_tag = concepts::tag<SentinelConcept>;
        using sized_sentinel_tag = concepts::tag<SizedSentinelConcept, sentinel_tag>;

        template<typename S, typename I>
        using sentinel_tag_of =
            concepts::tag_of<
                meta::list<SizedSentinelConcept, SentinelConcept>,
                S,
                I>;
        /// @}
    }
}

#ifdef _GLIBCXX_DEBUG
// HACKHACK: workaround underconstrained operator- for libstdc++ debug iterator wrapper
// by intentionally creating an ambiguity when the wrapped types don't support the
// necessary operation.
namespace __gnu_debug
{
    template<typename I1, typename I2, typename Seq>
    auto operator-(_Safe_iterator<I1, Seq> const &, _Safe_iterator<I2, Seq> const &) ->
        CPP_ret(void)(
            requires not ::ranges::SizedSentinel<I1, I2>) = delete;

    template<typename I1, typename Seq>
    auto operator-(_Safe_iterator<I1, Seq> const &, _Safe_iterator<I1, Seq> const &) ->
        CPP_ret(void)(
            requires not ::ranges::SizedSentinel<I1, I1>)= delete;
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
          : meta::not_c<SizedSentinel<I, S>>
        {};
    }
}
#endif // defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
