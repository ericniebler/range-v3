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

#ifndef RANGES_V3_ITERATOR_CONCEPTS_HPP
#define RANGES_V3_ITERATOR_CONCEPTS_HPP

#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/swap.hpp>

#ifdef _GLIBCXX_DEBUG
#include <debug/safe_iterator.h>
#endif

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename I>
        using iter_traits_t =
            if_then_t<is_std_iterator_traits_specialized_<I>, std::iterator_traits<I>, I>;

#if defined(_GLIBCXX_DEBUG)
        template<typename T, typename Seq>
        auto iter_concept_(__gnu_debug::_Safe_iterator<T *, Seq>, priority_tag<3>) ->
            ranges::contiguous_iterator_tag;
#endif
#if defined(__GLIBCXX__)
        template<typename T, typename Seq>
        auto iter_concept_(__gnu_cxx::__normal_iterator<T *, Seq>, priority_tag<3>) ->
            ranges::contiguous_iterator_tag;
#endif
#if defined(_LIBCPP_VERSION)
        template<typename T>
        auto iter_concept_(std::__wrap_iter<T *>, priority_tag<3>) ->
            ranges::contiguous_iterator_tag;
#endif
#if defined(_MSVC_STL_VERSION)
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_Array_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_Array_const_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_Vector_iterator> &&
                    !Same<bool, typename I::value_type>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_Vector_const_iterator> &&
                    !Same<bool, typename I::value_type>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_String_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_String_const_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>) ->
            CPP_ret(ranges::contiguous_iterator_tag)(
                requires Same<I, class I::_String_view_iterator>);
#endif
        template<typename T>
        auto iter_concept_(T *, priority_tag<3>) -> ranges::contiguous_iterator_tag;
        template<typename I>
        auto iter_concept_(I, priority_tag<2>) -> typename iter_traits_t<I>::iterator_concept;
        template<typename I>
        auto iter_concept_(I, priority_tag<1>) -> typename iter_traits_t<I>::iterator_category;
        template<typename I>
        auto iter_concept_(I, priority_tag<0>) ->
            enable_if_t<
                !is_std_iterator_traits_specialized_<I>,
                ranges::random_access_iterator_tag>;

        template<typename I>
        using iter_concept_t = decltype(iter_concept_(std::declval<I>(), priority_tag<3>{}));
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
            (!defer::True<disable_sized_sentinel<meta::_t<std::remove_cv<S>>, meta::_t<std::remove_cv<I>>>> &&
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

    ////////////////////////////////////////////////////////////////////////////////////////////
    // indirect_result_t
    template<typename Fun, typename... Is>
    using indirect_result_t =
        detail::enable_if_t<
            (bool) And<(bool) Readable<Is>...>,
            invoke_result_t<Fun, iter_reference_t<Is>...>>;

    /// \cond
    template<typename Fun, typename... Is>
    using indirect_invoke_result_t RANGES_DEPRECATED("Please switch to indirect_result_t") =
        indirect_result_t<Fun, Is...>;

    template<typename Fun, typename... Is>
    struct RANGES_DEPRECATED("Please switch to indirect_result_t") indirect_invoke_result
      : meta::defer<indirect_result_t, Fun, Is...>
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

    /// \cond
    namespace detail
    {
        CPP_def
        (
            template(typename T1, typename T2, typename T3, typename T4)
            concept CommonReference4_,
                Type<common_reference_t<T1, T2, T3, T4>> &&
                ConvertibleTo<T1, common_reference_t<T1, T2, T3, T4>> &&
                ConvertibleTo<T2, common_reference_t<T1, T2, T3, T4>> &&
                ConvertibleTo<T3, common_reference_t<T1, T2, T3, T4>> &&
                ConvertibleTo<T4, common_reference_t<T1, T2, T3, T4>>
            // axiom: all permutations of T1,T2,T3,T4 have the same
            // common reference type.
        );

        CPP_def
        (
            template(typename F, typename I)
            concept IndirectUnaryInvocable_,
                Readable<I> &&
                Invocable<F &, iter_value_t<I> &> &&
                Invocable<F &, iter_reference_t<I>> &&
                Invocable<F &, iter_common_reference_t<I>> &&
                CommonReference<
                    invoke_result_t<F &, iter_value_t<I> &>,
                    invoke_result_t<F &, iter_reference_t<I>>>
        );
    }
    /// \endcond

    CPP_def
    (
        template(typename F, typename I)
        concept IndirectUnaryInvocable,
            detail::IndirectUnaryInvocable_<F, I> &&
            CopyConstructible<F>
    );

    CPP_def
    (
        template(typename F, typename I)
        concept IndirectRegularUnaryInvocable,
            Readable<I> &&
            CopyConstructible<F> &&
            Invocable<F &, iter_value_t<I> &> &&
            Invocable<F &, iter_reference_t<I>> &&
            Invocable<F &, iter_common_reference_t<I>> &&
            CommonReference<
                invoke_result_t<F &, iter_value_t<I> &>,
                invoke_result_t<F &, iter_reference_t<I>>>
    );

    /// \cond
    // Non-standard indirect invocable concepts
    CPP_def
    (
        template(typename F, typename I1, typename I2)
        concept IndirectBinaryInvocable_,
            Readable<I1> && Readable<I2> &&
            CopyConstructible<F> &&
            Invocable<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            Invocable<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            Invocable<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            Invocable<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            Invocable<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>> &&
            detail::CommonReference4_<
                invoke_result_t<F &, iter_value_t<I1> &, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_value_t<I1> &, iter_reference_t<I2>>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_reference_t<I2>>>
    );

    CPP_def
    (
        template(typename F, typename I1, typename I2)
        concept IndirectRegularBinaryInvocable_,
            Readable<I1> && Readable<I2> &&
            CopyConstructible<F> &&
            RegularInvocable<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            RegularInvocable<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            RegularInvocable<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            RegularInvocable<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            RegularInvocable<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>> &&
            detail::CommonReference4_<
                invoke_result_t<F &, iter_value_t<I1> &, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_value_t<I1> &, iter_reference_t<I2>>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_reference_t<I2>>>
    );
    /// \endcond

    CPP_def
    (
        template(typename F, typename I)
        concept IndirectUnaryPredicate,
            Readable<I> &&
            CopyConstructible<F> &&
            Predicate<F &, iter_value_t<I> &> &&
            Predicate<F &, iter_reference_t<I>> &&
            Predicate<F &, iter_common_reference_t<I>>
    );

    CPP_def
    (
        template(typename F, typename I1, typename I2)
        concept IndirectBinaryPredicate,
            Readable<I1> && Readable<I2> &&
            CopyConstructible<F> &&
            Predicate<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            Predicate<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            Predicate<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            Predicate<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            Predicate<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    CPP_def
    (
        template(typename F, typename I1, typename I2 = I1)
        (concept IndirectRelation)(F, I1, I2),
            Readable<I1> && Readable<I2> &&
            CopyConstructible<F> &&
            Relation<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            Relation<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            Relation<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            Relation<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            Relation<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    CPP_def
    (
        template(class F, class I1, class I2 = I1)
        (concept IndirectStrictWeakOrder)(F, I1, I2),
            Readable<I1> && Readable<I2> &&
            CopyConstructible<F> &&
            StrictWeakOrder<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            StrictWeakOrder<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            StrictWeakOrder<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            StrictWeakOrder<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            StrictWeakOrder<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

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

        template<typename Proj>
        struct select_projected_
        {
            template<typename I>
            using apply =
                detail::enable_if_t<
                    (bool) IndirectRegularUnaryInvocable<Proj, I>,
                    detail::projected_<I, Proj>>;
        };

        template<>
        struct select_projected_<identity>
        {
            template<typename I>
            using apply = detail::enable_if_t<(bool) Readable<I>, I>;
        };
    }
    /// \endcond

    template<typename I, typename Proj>
    using projected =
        typename detail::select_projected_<Proj>::template apply<I>;

    template<typename I, typename Proj>
    struct incrementable_traits<detail::projected_<I, Proj>>
      : incrementable_traits<I>
    {};

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
            Writable<O, iter_value_t<I>> &&
            Movable<iter_value_t<I>> &&
            Constructible<iter_value_t<I>, iter_rvalue_reference_t<I>> &&
            Assignable<iter_value_t<I> &, iter_rvalue_reference_t<I>>
    );

    CPP_def
    (
        template(typename I, typename O)
        concept IndirectlyCopyable,
            Readable<I> &&
            Writable<O, iter_reference_t<I>>
    );

    CPP_def
    (
        template(typename I, typename O)
        concept IndirectlyCopyableStorable,
            IndirectlyCopyable<I, O> &&
            Writable<O, iter_value_t<I> const &> &&
            Copyable<iter_value_t<I>> &&
            Constructible<iter_value_t<I>, iter_reference_t<I>> &&
            Assignable<iter_value_t<I> &, iter_reference_t<I>>
    );

    CPP_def
    (
        template(typename I1, typename I2 = I1)
        (concept IndirectlySwappable)(I1, I2),
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
        template(typename I1, typename I2, typename C,
            typename P1 = identity, typename P2 = identity)
        (concept IndirectlyComparable)(I1, I2, C, P1, P2),
            IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>
    );

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
        template(typename I0, typename I1, typename Out, typename C = less,
            typename P0 = identity, typename P1 = identity)
        (concept Mergeable)(I0, I1, Out, C, P0, P1),
            InputIterator<I0> &&
            InputIterator<I1> &&
            WeaklyIncrementable<Out> &&
            IndirectlyCopyable<I0, Out> &&
            IndirectlyCopyable<I1, Out> &&
            IndirectStrictWeakOrder<C, projected<I0, P0>, projected<I1, P1>>
    );

    CPP_def
    (
        template(typename I, typename C = less, typename P = identity)
        (concept Sortable)(I, C, P),
            Permutable<I> &&
            IndirectStrictWeakOrder<C, projected<I, P>>
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
    template<typename S, typename I>
    /*inline*/ constexpr bool
    disable_sized_sentinel<std::reverse_iterator<S>, std::reverse_iterator<I>> =
      !static_cast<bool>(SizedSentinel<I, S>);
}
#endif // defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)

#endif // RANGES_V3_ITERATOR_CONCEPTS_HPP
