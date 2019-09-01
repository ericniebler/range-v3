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

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/access.hpp>
#include <range/v3/iterator/traits.hpp>

#ifdef _GLIBCXX_DEBUG
#include <debug/safe_iterator.h>
#endif

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{

    /// \cond
    namespace detail
    {
        template<typename I>
        using iter_traits_t = if_then_t<is_std_iterator_traits_specialized_v<I>,
                                        std::iterator_traits<I>, I>;

#if defined(_GLIBCXX_DEBUG)
        template<typename I, typename T, typename Seq>
        auto iter_concept_(__gnu_debug::_Safe_iterator<T *, Seq>, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, __gnu_debug::_Safe_iterator<T *, Seq>>);
#endif
#if defined(__GLIBCXX__)
        template<typename I, typename T, typename Seq>
        auto iter_concept_(__gnu_cxx::__normal_iterator<T *, Seq>, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, __gnu_cxx::__normal_iterator<T *, Seq>>);
#endif
#if defined(_LIBCPP_VERSION)
        template<typename I, typename T>
        auto iter_concept_(std::__wrap_iter<T *>, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, std::__wrap_iter<T *>>);
#endif
#if defined(_MSVC_STL_VERSION)
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_Array_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_Array_const_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_Vector_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_Vector_const_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_String_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_String_const_iterator>);
        template<typename I>
        auto iter_concept_(I, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, class I::_String_view_iterator>);
#endif
        template<typename I, typename T>
        auto iter_concept_(T *, priority_tag<3>)
            -> CPP_ret(ranges::contiguous_iterator_tag)( //
                requires same_as<I, T *>);
        template<typename I>
        auto iter_concept_(I, priority_tag<2>) ->
            typename iter_traits_t<I>::iterator_concept;
        template<typename I>
        auto iter_concept_(I, priority_tag<1>) ->
            typename iter_traits_t<I>::iterator_category;
        template<typename I>
        auto iter_concept_(I, priority_tag<0>)
            -> enable_if_t<!is_std_iterator_traits_specialized_v<I>,
                           std::random_access_iterator_tag>;

        template<typename I>
        using iter_concept_t =
            decltype(iter_concept_<I>(std::declval<I>(), priority_tag<3>{}));

        using ::concepts::detail::weakly_equality_comparable_with_;
    } // namespace detail
      /// \endcond

    // clang-format off
    CPP_def
    (
        template(typename I)
        concept readable,
            common_reference_with<iter_reference_t<I> &&,
                                  iter_value_t<std::remove_reference_t<I>> &> &&
            common_reference_with<iter_reference_t<I> &&,
                                  iter_rvalue_reference_t<I> &&> &&
            common_reference_with<iter_rvalue_reference_t<I> &&,
                                  iter_value_t<std::remove_reference_t<I>> const &>
    );

    CPP_def
    (
        template(typename Out, typename T)
        concept writable,
            requires (Out &&o, T &&t)
            (
                *o = static_cast<T &&>(t),
                *((Out &&) o) = static_cast<T &&>(t),
                const_cast<iter_reference_t<Out> const &&>(*o) = static_cast<T &&>(t),
                const_cast<iter_reference_t<Out> const &&>(*((Out &&) o)) =
                    static_cast<T &&>(t)
            )
    );
    // clang-format on

    /// \cond
    namespace detail
    {
        template<typename D>
        RANGES_INLINE_VAR constexpr bool _is_integer_like_ = std::is_integral<D>::value;

        // clang-format off
        CPP_def
        (
            template(typename D)
            concept integer_like_,
                _is_integer_like_<D>
                // TODO additional syntactic and semantic requirements
        );

#ifdef RANGES_WORKAROUND_MSVC_792338
        CPP_def
        (
            template(typename D)
            concept signed_integer_like_,
                concepts::type<decltype(std::integral_constant<bool, (D(-1) < D(0))>{})> &&
                std::integral_constant<bool, (D(-1) < D(0))>::value
        );
#else // ^^^ workaround / no workaround vvv
        CPP_def
        (
            template(typename D)
            concept signed_integer_like_,
                integer_like_<D> &&
                concepts::type<std::integral_constant<bool, (D(-1) < D(0))>> &&
                std::integral_constant<bool, (D(-1) < D(0))>::value
        );
#endif // RANGES_WORKAROUND_MSVC_792338
       // clang-format on
    } // namespace detail
      /// \endcond

    // clang-format off
    CPP_def
    (
        template(typename I)
        concept weakly_incrementable,
            requires (I i)
            (
                ++i,
                i++,
                concepts::requires_<same_as<I&, decltype(++i)>>
            ) &&
            concepts::type<iter_difference_t<I>> &&
            detail::signed_integer_like_<iter_difference_t<I>> &&
            semiregular<I>
    );

    CPP_def
    (
        template(typename I)
        concept incrementable,
            requires (I i)
            (
                concepts::requires_<same_as<I, decltype(i++)>>
            ) &&
            regular<I> && weakly_incrementable<I>
    );

    CPP_def
    (
        template(typename I)
        concept input_or_output_iterator,
            requires (I i)
            (
                *i
            ) &&
            weakly_incrementable<I>
    );

    CPP_def
    (
        template(typename S, typename I)
        concept sentinel_for,
            semiregular<S> && input_or_output_iterator<I> &&
            detail::weakly_equality_comparable_with_<S, I>
    );

    CPP_def
    (
        template(typename S, typename I)
        concept sized_sentinel_for,
            requires (S const &s, I const &i)
            (
                s - i,
                i - s,
                concepts::requires_<same_as<iter_difference_t<I>, decltype(s - i)>>,
                concepts::requires_<same_as<iter_difference_t<I>, decltype(i - s)>>
            ) &&
            // Short-circuit the test for sentinel_for if we're emulating concepts:
            (!defer::is_true<disable_sized_sentinel<meta::_t<std::remove_cv<S>>, meta::_t<std::remove_cv<I>>>> &&
            defer::sentinel_for<S, I>)
    );

    CPP_def
    (
        template(typename Out, typename T)
        concept output_iterator,
            requires (Out o, T &&t)
            (
                *o++ = static_cast<T &&>(t)
            ) &&
            input_or_output_iterator<Out> && writable<Out, T>
    );

    CPP_def
    (
        template(typename I)
        concept input_iterator,
            input_or_output_iterator<I> && readable<I> &&
            derived_from<detail::iter_concept_t<I>, std::input_iterator_tag>
    );

    CPP_def
    (
        template(typename I)
        concept forward_iterator,
            input_iterator<I> && incrementable<I> &&
            sentinel_for<I, I> &&
            derived_from<detail::iter_concept_t<I>, std::forward_iterator_tag>
    );

    CPP_def
    (
        template(typename I)
        concept bidirectional_iterator,
            requires (I i)
            (
                --i,
                i--,
                concepts::requires_<same_as<I&, decltype(--i)>>,
                concepts::requires_<same_as<I, decltype(i--)>>
            ) &&
            forward_iterator<I> &&
            derived_from<detail::iter_concept_t<I>, std::bidirectional_iterator_tag>
    );

    CPP_def
    (
        template(typename I)
        concept random_access_iterator,
            requires (I i, iter_difference_t<I> n)
            (
                i + n,
                n + i,
                i - n,
                i += n,
                i -= n,
                concepts::requires_<same_as<decltype(i + n), I>>,
                concepts::requires_<same_as<decltype(n + i), I>>,
                concepts::requires_<same_as<decltype(i - n), I>>,
                concepts::requires_<same_as<decltype(i += n), I&>>,
                concepts::requires_<same_as<decltype(i -= n), I&>>,
                concepts::requires_<same_as<decltype(i[n]), iter_reference_t<I>>>
            ) &&
            bidirectional_iterator<I> &&
            totally_ordered<I> &&
            sized_sentinel_for<I, I> &&
            derived_from<detail::iter_concept_t<I>, std::random_access_iterator_tag>
    );

    CPP_def
    (
        template(typename I)
        concept contiguous_iterator,
            random_access_iterator<I> &&
            derived_from<detail::iter_concept_t<I>, ranges::contiguous_iterator_tag> &&
            std::is_lvalue_reference<iter_reference_t<I>>::value &&
            same_as<iter_value_t<I>, uncvref_t<iter_reference_t<I>>>
    );
    // clang-format on

    /////////////////////////////////////////////////////////////////////////////////////
    // iterator_tag_of
    template<typename T>
    using iterator_tag_of = concepts::tag_of<
        meta::list<contiguous_iterator_concept, random_access_iterator_concept,
                   bidirectional_iterator_concept, forward_iterator_concept,
                   input_iterator_concept>,
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
            static std::input_iterator_tag test(detail::input_iterator_tag_);
            static std::forward_iterator_tag test(detail::forward_iterator_tag_);
            static std::bidirectional_iterator_tag test(
                detail::bidirectional_iterator_tag_);
            static std::random_access_iterator_tag test(
                detail::random_access_iterator_tag_);
            static ranges::contiguous_iterator_tag test(detail::contiguous_iterator_tag_);

        public:
            using type = decltype(iterator_category_::test(iterator_tag_of<I>{}));
        };

        template<typename T>
        using iterator_category =
            iterator_category_<meta::_t<std::remove_const<T>>,
                               (bool)input_iterator<meta::_t<std::remove_const<T>>>>;
    } // namespace detail
    /// \endcond

    /// \cond
    // Generally useful to know if an iterator is single-pass or not:
    // clang-format off
    CPP_def
    (
        template(typename I)
        concept single_pass_iterator_,
            input_or_output_iterator<I> && !forward_iterator<I>
    );
    // clang-format on
    /// \endcond

    ////////////////////////////////////////////////////////////////////////////////////////////
    // indirect_result_t
    template<typename Fun, typename... Is>
    using indirect_result_t =
        detail::enable_if_t<(bool)and_v<(bool)readable<Is>...>,
                            invoke_result_t<Fun, iter_reference_t<Is>...>>;

    /// \cond
    namespace detail
    {
        // clang-format off
        CPP_def
        (
            template(typename T1, typename T2, typename T3, typename T4)
            concept common_reference_with_4_,
                concepts::type<common_reference_t<T1, T2, T3, T4>> &&
                convertible_to<T1, common_reference_t<T1, T2, T3, T4>> &&
                convertible_to<T2, common_reference_t<T1, T2, T3, T4>> &&
                convertible_to<T3, common_reference_t<T1, T2, T3, T4>> &&
                convertible_to<T4, common_reference_t<T1, T2, T3, T4>>
            // axiom: all permutations of T1,T2,T3,T4 have the same
            // common reference type.
        );

        CPP_def
        (
            template(typename F, typename I)
            concept indirectly_unary_invocable_,
                readable<I> &&
                invocable<F &, iter_value_t<I> &> &&
                invocable<F &, iter_reference_t<I>> &&
                invocable<F &, iter_common_reference_t<I>> &&
                common_reference_with<
                    invoke_result_t<F &, iter_value_t<I> &>,
                    invoke_result_t<F &, iter_reference_t<I>>>
        );
        // clang-format on
    } // namespace detail
      /// \endcond

    // clang-format off
    CPP_def
    (
        template(typename F, typename I)
        concept indirectly_unary_invocable,
            detail::indirectly_unary_invocable_<F, I> &&
            copy_constructible<F>
    );

    CPP_def
    (
        template(typename F, typename I)
        concept indirectly_regular_unary_invocable,
            readable<I> &&
            copy_constructible<F> &&
            regular_invocable<F &, iter_value_t<I> &> &&
            regular_invocable<F &, iter_reference_t<I>> &&
            regular_invocable<F &, iter_common_reference_t<I>> &&
            common_reference_with<
                invoke_result_t<F &, iter_value_t<I> &>,
                invoke_result_t<F &, iter_reference_t<I>>>
    );

    /// \cond
    // Non-standard indirect invocable concepts
    CPP_def
    (
        template(typename F, typename I1, typename I2)
        concept indirectly_binary_invocable_,
            readable<I1> && readable<I2> &&
            copy_constructible<F> &&
            invocable<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            invocable<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            invocable<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            invocable<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            invocable<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>> &&
            detail::common_reference_with_4_<
                invoke_result_t<F &, iter_value_t<I1> &, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_value_t<I1> &, iter_reference_t<I2>>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_reference_t<I2>>>
    );

    CPP_def
    (
        template(typename F, typename I1, typename I2)
        concept indirectly_regular_binary_invocable_,
            readable<I1> && readable<I2> &&
            copy_constructible<F> &&
            regular_invocable<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            regular_invocable<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            regular_invocable<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            regular_invocable<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            regular_invocable<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>> &&
            detail::common_reference_with_4_<
                invoke_result_t<F &, iter_value_t<I1> &, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_value_t<I1> &, iter_reference_t<I2>>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_value_t<I2> &>,
                invoke_result_t<F &, iter_reference_t<I1>, iter_reference_t<I2>>>
    );
    /// \endcond

    CPP_def
    (
        template(typename F, typename I)
        concept indirect_unary_predicate,
            readable<I> &&
            copy_constructible<F> &&
            predicate<F &, iter_value_t<I> &> &&
            predicate<F &, iter_reference_t<I>> &&
            predicate<F &, iter_common_reference_t<I>>
    );

    CPP_def
    (
        template(typename F, typename I1, typename I2)
        concept indirect_binary_predicate_,
            readable<I1> && readable<I2> &&
            copy_constructible<F> &&
            predicate<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            predicate<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            predicate<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            predicate<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            predicate<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    CPP_def
    (
        template(typename F, typename I1, typename I2 = I1)
        (concept indirect_relation)(F, I1, I2),
            readable<I1> && readable<I2> &&
            copy_constructible<F> &&
            relation<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            relation<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            relation<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            relation<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            relation<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    CPP_def
    (
        template(class F, class I1, class I2 = I1)
        (concept indirect_strict_weak_order)(F, I1, I2),
            readable<I1> && readable<I2> &&
            copy_constructible<F> &&
            strict_weak_order<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
            strict_weak_order<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
            strict_weak_order<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
            strict_weak_order<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
            strict_weak_order<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );
    // clang-format on

    ////////////////////////////////////////////////////////////////////////////////////////////
    // projected struct, for "projecting" a readable with a unary callable
    /// \cond
    namespace detail
    {
        RANGES_DIAGNOSTIC_PUSH
        RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_INTERNAL
        template<typename I, typename Proj>
        struct projected_
        {
            using reference = indirect_result_t<Proj &, I>;
            using value_type = uncvref_t<reference>;
            reference operator*() const;
        };
        RANGES_DIAGNOSTIC_POP

        template<typename Proj>
        struct select_projected_
        {
            template<typename I>
            using apply =
                detail::enable_if_t<(bool)indirectly_regular_unary_invocable<Proj, I>,
                                    detail::projected_<I, Proj>>;
        };

        template<>
        struct select_projected_<identity>
        {
            template<typename I>
            using apply = detail::enable_if_t<(bool)readable<I>, I>;
        };
    } // namespace detail
    /// \endcond

    template<typename I, typename Proj>
    using projected = typename detail::select_projected_<Proj>::template apply<I>;

    template<typename I, typename Proj>
    struct incrementable_traits<detail::projected_<I, Proj>> : incrementable_traits<I>
    {};

    // clang-format off
    CPP_def
    (
        template(typename I, typename O)
        concept indirectly_movable,
            readable<I> && writable<O, iter_rvalue_reference_t<I>>
    );

    CPP_def
    (
        template(typename I, typename O)
        concept indirectly_movable_storable,
            indirectly_movable<I, O> &&
            writable<O, iter_value_t<I>> &&
            movable<iter_value_t<I>> &&
            constructible_from<iter_value_t<I>, iter_rvalue_reference_t<I>> &&
            assignable_from<iter_value_t<I> &, iter_rvalue_reference_t<I>>
    );

    CPP_def
    (
        template(typename I, typename O)
        concept indirectly_copyable,
            readable<I> &&
            writable<O, iter_reference_t<I>>
    );

    CPP_def
    (
        template(typename I, typename O)
        concept indirectly_copyable_storable,
            indirectly_copyable<I, O> &&
            writable<O, iter_value_t<I> const &> &&
            copyable<iter_value_t<I>> &&
            constructible_from<iter_value_t<I>, iter_reference_t<I>> &&
            assignable_from<iter_value_t<I> &, iter_reference_t<I>>
    );

    CPP_def
    (
        template(typename I1, typename I2 = I1)
        (concept indirectly_swappable)(I1, I2),
            requires (I1 && i1, I2 && i2)
            (
                ranges::iter_swap((I1 &&) i1, (I2 &&) i2),
                ranges::iter_swap((I1 &&) i1, (I1 &&) i1),
                ranges::iter_swap((I2 &&) i2, (I2 &&) i2),
                ranges::iter_swap((I2 &&) i2, (I1 &&) i1)
            ) &&
            readable<I1> && readable<I2>
    );

    CPP_def
    (
        template(typename I1, typename I2, typename C,
            typename P1 = identity, typename P2 = identity)
        (concept indirectly_comparable)(I1, I2, C, P1, P2),
            indirect_relation<C, projected<I1, P1>, projected<I2, P2>>
    );

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Composite concepts for use defining algorithms:
    CPP_def
    (
        template(typename I)
        concept permutable,
            forward_iterator<I> &&
            indirectly_swappable<I, I> &&
            indirectly_movable_storable<I, I>
    );

    CPP_def
    (
        template(typename I0, typename I1, typename Out, typename C = less,
            typename P0 = identity, typename P1 = identity)
        (concept mergeable)(I0, I1, Out, C, P0, P1),
            input_iterator<I0> &&
            input_iterator<I1> &&
            weakly_incrementable<Out> &&
            indirectly_copyable<I0, Out> &&
            indirectly_copyable<I1, Out> &&
            indirect_strict_weak_order<C, projected<I0, P0>, projected<I1, P1>>
    );

    CPP_def
    (
        template(typename I, typename C = less, typename P = identity)
        (concept sortable)(I, C, P),
            permutable<I> &&
            indirect_strict_weak_order<C, projected<I, P>>
    );
    // clang-format on

    using sentinel_tag = concepts::tag<sentinel_for_concept>;
    using sized_sentinel_tag = concepts::tag<sized_sentinel_for_concept, sentinel_tag>;

    template<typename S, typename I>
    using sentinel_tag_of =
        concepts::tag_of<meta::list<sized_sentinel_for_concept, sentinel_for_concept>, S,
                         I>;

    // Deprecated things:
    /// \cond
    template<typename I>
    using iterator_category RANGES_DEPRECATED(
        "iterator_category is deprecated. Use the iterator concepts instead") =
        detail::iterator_category<I>;

    template<typename I>
    using iterator_category_t RANGES_DEPRECATED(
        "iterator_category_t is deprecated. Use the iterator concepts instead") =
        meta::_t<detail::iterator_category<I>>;

    template<typename Fun, typename... Is>
    using indirect_invoke_result_t RANGES_DEPRECATED(
        "Please switch to indirect_result_t") = indirect_result_t<Fun, Is...>;

    template<typename Fun, typename... Is>
    struct RANGES_DEPRECATED("Please switch to indirect_result_t") indirect_invoke_result
      : meta::defer<indirect_result_t, Fun, Is...>
    {};

    template<typename Sig>
    struct indirect_result_of
    {};

    template<typename Fun, typename... Is>
    struct RANGES_DEPRECATED("Please switch to indirect_result_t")
        indirect_result_of<Fun(Is...)> : meta::defer<indirect_result_t, Fun, Is...>
    {};

    template<typename Sig>
    using indirect_result_of_t RANGES_DEPRECATED("Please switch to indirect_result_t") =
        meta::_t<indirect_result_of<Sig>>;
    /// \endcond

    namespace cpp20
    {
        using ranges::bidirectional_iterator;
        using ranges::contiguous_iterator;
        using ranges::forward_iterator;
        using ranges::incrementable;
        using ranges::indirect_relation;
        using ranges::indirect_result_t;
        using ranges::indirect_strict_weak_order;
        using ranges::indirect_unary_predicate;
        using ranges::indirectly_comparable;
        using ranges::indirectly_copyable;
        using ranges::indirectly_copyable_storable;
        using ranges::indirectly_movable;
        using ranges::indirectly_movable_storable;
        using ranges::indirectly_regular_unary_invocable;
        using ranges::indirectly_swappable;
        using ranges::indirectly_unary_invocable;
        using ranges::input_iterator;
        using ranges::input_or_output_iterator;
        using ranges::mergeable;
        using ranges::output_iterator;
        using ranges::permutable;
        using ranges::projected;
        using ranges::random_access_iterator;
        using ranges::readable;
        using ranges::sentinel_for;
        using ranges::sortable;
        using ranges::weakly_incrementable;
        using ranges::writable;
    } // namespace cpp20
    /// @}
} // namespace ranges

#ifdef _GLIBCXX_DEBUG
// HACKHACK: workaround underconstrained operator- for libstdc++ debug iterator wrapper
// by intentionally creating an ambiguity when the wrapped types don't support the
// necessary operation.
namespace __gnu_debug
{
    template<typename I1, typename I2, typename Seq>
    auto operator-(_Safe_iterator<I1, Seq> const &, _Safe_iterator<I2, Seq> const &)
        -> CPP_ret(void)( //
            requires(!::ranges::sized_sentinel_for<I1, I2>)) = delete;

    template<typename I1, typename Seq>
    auto operator-(_Safe_iterator<I1, Seq> const &, _Safe_iterator<I1, Seq> const &)
        -> CPP_ret(void)( //
            requires(!::ranges::sized_sentinel_for<I1, I1>)) = delete;
} // namespace __gnu_debug
#endif

#if defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)
// HACKHACK: workaround libc++ (https://llvm.org/bugs/show_bug.cgi?id=28421)
// and libstdc++ (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71771)
// underconstrained operator- for reverse_iterator by disabling sized_sentinel_for
// when the base iterators do not model sized_sentinel_for.
namespace ranges
{
    template<typename S, typename I>
    /*inline*/ constexpr bool
        disable_sized_sentinel<std::reverse_iterator<S>, std::reverse_iterator<I>> =
            !static_cast<bool>(sized_sentinel_for<I, S>);
}
#endif // defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)

#endif // RANGES_V3_ITERATOR_CONCEPTS_HPP
