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

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-iterator-concepts
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

        template<typename I>
        using readable_types_t =
            meta::list<iter_value_t<I>, iter_reference_t<I>, iter_rvalue_reference_t<I>>;
    } // namespace detail
      /// \endcond

    // clang-format off
    template<typename I>
    CPP_concept_fragment(readable_, (I),
        common_reference_with<iter_reference_t<I> &&, iter_value_t<I> &> &&
        common_reference_with<iter_reference_t<I> &&,
                              iter_rvalue_reference_t<I> &&> &&
        common_reference_with<iter_rvalue_reference_t<I> &&, iter_value_t<I> const &>
    );

    template<typename I>
    CPP_concept_bool readable =
        CPP_requires ((uncvref_t<I> const) i, (uncvref_t<I>) j) //
        (
            // { *i } -> same_as<iter_reference_t<I>>;
            concepts::requires_<same_as<iter_reference_t<decltype(i)>,
                                        iter_reference_t<decltype(j)>>>,
            // { iter_move(i) } -> same_as<iter_rvalue_reference_t<I>>;
            concepts::requires_<same_as<iter_rvalue_reference_t<decltype(i)>,
                                        iter_rvalue_reference_t<decltype(j)>>>
        ) &&
        CPP_fragment(readable_, uncvref_t<I>);

    template<typename Out, typename T>
    CPP_concept_bool writable =
        CPP_requires ((Out &&) o, (T &&) t) //
        (
            *o = CPP_fwd(t),
            *CPP_fwd(o) = CPP_fwd(t),
            const_cast<iter_reference_t<decltype(o)> const &&>(*o) = CPP_fwd(t),
            const_cast<iter_reference_t<decltype(o)> const &&>(*CPP_fwd(o)) = CPP_fwd(t)
        );
    // clang-format on

    /// \cond
    namespace detail
    {
#if RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
        template<typename D>
        inline constexpr bool _is_integer_like_ = std::is_integral<D>::value;
#else
        template<typename D, typename = void>
        constexpr bool _is_integer_like_ = std::is_integral<D>::value;
#endif

        // clang-format off
        template<typename D>
        CPP_concept_bool integer_like_ =
            _is_integer_like_<D>;
            // TODO additional syntactic and semantic requirements

#ifdef RANGES_WORKAROUND_MSVC_792338
        template<typename D>
        CPP_concept_fragment(signed_integer_like_frag_, (D),
            integer_like_<D> &&
            concepts::type<decltype(std::integral_constant<bool, (D(-1) < D(0))>{})> &&
            std::integral_constant<bool, (D(-1) < D(0))>::value
        );
#else // ^^^ workaround / no workaround vvv
        template<typename D>
        CPP_concept_fragment(signed_integer_like_frag_, (D),
            integer_like_<D> &&
            concepts::type<std::integral_constant<bool, (D(-1) < D(0))>> &&
            std::integral_constant<bool, (D(-1) < D(0))>::value
        );
#endif // RANGES_WORKAROUND_MSVC_792338
        template<typename D>
        CPP_concept_bool signed_integer_like_ =
            CPP_fragment(detail::signed_integer_like_frag_, D);
        // clang-format on
    } // namespace detail
      /// \endcond

    // clang-format off
    template<typename I>
    CPP_concept_fragment(weakly_incrementable_, (I),
        concepts::type<iter_difference_t<I>> &&
        detail::signed_integer_like_<iter_difference_t<I>>
    );

    template<typename I>
    CPP_concept_bool weakly_incrementable =
        semiregular<I> &&
        CPP_fragment(ranges::weakly_incrementable_, I) &&
        CPP_requires ((I) i) //
        (
            ++i,
            i++,
            concepts::requires_<same_as<I&, decltype(++i)>>
        );

    template<typename I>
    CPP_concept_bool incrementable =
        regular<I> && weakly_incrementable<I> &&
        CPP_requires ((I) i) //
        (
            concepts::requires_<same_as<I, decltype(i++)>>
        );

    template<typename I>
    CPP_concept_bool input_or_output_iterator =
        weakly_incrementable<I> &&
        detail::dereferenceable_<I&>;

    template<typename S, typename I>
    CPP_concept_bool sentinel_for =
        semiregular<S> && input_or_output_iterator<I> &&
        detail::weakly_equality_comparable_with_<S, I>;

    namespace defer
    {
        template<typename S, typename I>
        CPP_concept sentinel_for =
            CPP_defer(ranges::sentinel_for, S, I);
    } // namespace defer

    template<typename S, typename I>
    CPP_concept_bool sized_sentinel_for =
        CPP_requires ((S const &) s, (I const &) i) //
        (
            s - i,
            i - s,
            concepts::requires_<same_as<iter_difference_t<decltype(i)>, decltype(s - i)>>,
            concepts::requires_<same_as<iter_difference_t<decltype(i)>, decltype(i - s)>>
        ) &&
        // Short-circuit the test for sentinel_for if we're emulating concepts:
        bool(!defer::is_true<disable_sized_sentinel<std::remove_cv_t<S>,
                                                    std::remove_cv_t<I>>> &&
            defer::sentinel_for<S, I>);

    template<typename Out, typename T>
    CPP_concept_bool output_iterator =
        input_or_output_iterator<Out> && writable<Out, T> &&
        CPP_requires ((Out) o, (T &&) t) //
        (
            *o++ = CPP_fwd(t)
        );

    template<typename I, typename Tag>
    CPP_concept_fragment(with_category_, (I, Tag),
        derived_from<detail::iter_concept_t<I>, Tag>
    );

    template<typename I>
    CPP_concept_bool input_iterator =
        input_or_output_iterator<I> && readable<I> &&
        CPP_fragment(ranges::with_category_, I, std::input_iterator_tag);

    template<typename I>
    CPP_concept_bool forward_iterator =
        input_iterator<I> && incrementable<I> &&
        sentinel_for<I, I> &&
        CPP_fragment(ranges::with_category_, I, std::forward_iterator_tag);

    template<typename I>
    CPP_concept_bool bidirectional_iterator =
        CPP_requires ((I) i) //
        (
            --i,
            i--,
            concepts::requires_<same_as<I&, decltype(--i)>>,
            concepts::requires_<same_as<I, decltype(i--)>>
        ) &&
        forward_iterator<I> &&
        CPP_fragment(ranges::with_category_, I, std::bidirectional_iterator_tag);

    template<typename I>
    CPP_concept_bool random_access_iterator =
        CPP_requires_ ((I) i, (iter_difference_t<CPP_type(I)>) n,
                       (iter_reference_t<CPP_type(I)>&))
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
            concepts::requires_<same_as<decltype(i[n]), iter_reference_t<CPP_type(I)>>>
        ) &&
        bidirectional_iterator<I> &&
        totally_ordered<I> &&
        sized_sentinel_for<I, I> &&
        CPP_fragment(ranges::with_category_, I, std::random_access_iterator_tag);

    template<typename I>
    CPP_concept_fragment(contiguous_iterator_, (I),
        std::is_lvalue_reference<iter_reference_t<I>>::value &&
        same_as<iter_value_t<I>, uncvref_t<iter_reference_t<I>>> &&
        derived_from<detail::iter_concept_t<I>, ranges::contiguous_iterator_tag>
    );

    template<typename I>
    CPP_concept_bool contiguous_iterator =
        random_access_iterator<I> &&
        CPP_fragment(ranges::contiguous_iterator_, I);
    // clang-format on

    /////////////////////////////////////////////////////////////////////////////////////
    // iterator_tag_of
    template<typename Rng>
    using iterator_tag_of =                              //
        std::enable_if_t<                                //
            input_iterator<Rng>,                         //
            detail::if_then_t<                           //
                contiguous_iterator<Rng>,                //
                ranges::contiguous_iterator_tag,         //
                detail::if_then_t<                       //
                    random_access_iterator<Rng>,         //
                    std::random_access_iterator_tag,     //
                    detail::if_then_t<                   //
                        bidirectional_iterator<Rng>,     //
                        std::bidirectional_iterator_tag, //
                        detail::if_then_t<               //
                            forward_iterator<Rng>,       //
                            std::forward_iterator_tag,   //
                            std::input_iterator_tag>>>>>;

    /// \cond
    namespace detail
    {
        template<typename, bool>
        struct iterator_category_
        {};

        template<typename I>
        struct iterator_category_<I, true>
        {
            using type = iterator_tag_of<I>;
        };

        template<typename T, typename U = meta::_t<std::remove_const<T>>>
        using iterator_category = iterator_category_<U, (bool)input_iterator<U>>;
    } // namespace detail
    /// \endcond

    /// \cond
    // Generally useful to know if an iterator is single-pass or not:
    // clang-format off
    template<typename I>
    CPP_concept_bool single_pass_iterator_ =
        input_or_output_iterator<I> && !forward_iterator<I>;
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
        template<typename T1, typename T2, typename T3, typename T4>
        CPP_concept_fragment(common_reference_with_4_, (T1, T2, T3, T4),
            concepts::type<common_reference_t<T1, T2, T3, T4>> &&
            convertible_to<T1, common_reference_t<T1, T2, T3, T4>> &&
            convertible_to<T2, common_reference_t<T1, T2, T3, T4>> &&
            convertible_to<T3, common_reference_t<T1, T2, T3, T4>> &&
            convertible_to<T4, common_reference_t<T1, T2, T3, T4>>
        );
        // axiom: all permutations of T1,T2,T3,T4 have the same
        // common reference type.

        template<typename F, typename I>
        CPP_concept_fragment(indirectly_unary_invocable_frag_, (F, I),
            invocable<F &, iter_value_t<I> &> &&
            invocable<F &, iter_reference_t<I>> &&
            invocable<F &, iter_common_reference_t<I>> &&
            common_reference_with<
                invoke_result_t<F &, iter_value_t<I> &>,
                invoke_result_t<F &, iter_reference_t<I>>>
        );

        template<typename F, typename I>
        CPP_concept_bool indirectly_unary_invocable_ =
            readable<I> &&
            CPP_fragment(detail::indirectly_unary_invocable_frag_, F, I);
        // clang-format on
    } // namespace detail
      /// \endcond

    // clang-format off
    template<typename F, typename I>
    CPP_concept_bool indirectly_unary_invocable =
        detail::indirectly_unary_invocable_<F, I> &&
        copy_constructible<F>;

    template<typename F, typename I>
    CPP_concept_fragment(indirectly_regular_unary_invocable_, (F, I),
        regular_invocable<F &, iter_value_t<I> &> &&
        regular_invocable<F &, iter_reference_t<I>> &&
        regular_invocable<F &, iter_common_reference_t<I>> &&
        common_reference_with<
            invoke_result_t<F &, iter_value_t<I> &>,
            invoke_result_t<F &, iter_reference_t<I>>>
    );

    template<typename F, typename I>
    CPP_concept_bool indirectly_regular_unary_invocable =
        readable<I> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirectly_regular_unary_invocable_, F, I);

    /// \cond
    // Non-standard indirect invocable concepts
    template<typename F, typename I1, typename I2>
    CPP_concept_fragment(indirectly_binary_invocable_frag_, (F, I1, I2),
        invocable<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
        invocable<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
        invocable<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
        invocable<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
        invocable<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>> &&
        CPP_fragment(detail::common_reference_with_4_,
            invoke_result_t<F &, iter_value_t<I1> &, iter_value_t<I2> &>,
            invoke_result_t<F &, iter_value_t<I1> &, iter_reference_t<I2>>,
            invoke_result_t<F &, iter_reference_t<I1>, iter_value_t<I2> &>,
            invoke_result_t<F &, iter_reference_t<I1>, iter_reference_t<I2>>)
    );

    template<typename F, typename I1, typename I2>
    CPP_concept_bool indirectly_binary_invocable_ =
        readable<I1> && readable<I2> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirectly_binary_invocable_frag_, F, I1, I2);

    template<typename F, typename I1, typename I2>
    CPP_concept_fragment(indirectly_regular_binary_invocable_frag_, (F, I1, I2),
        regular_invocable<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
        regular_invocable<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
        regular_invocable<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
        regular_invocable<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
        regular_invocable<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>> &&
        CPP_fragment(detail::common_reference_with_4_,
            invoke_result_t<F &, iter_value_t<I1> &, iter_value_t<I2> &>,
            invoke_result_t<F &, iter_value_t<I1> &, iter_reference_t<I2>>,
            invoke_result_t<F &, iter_reference_t<I1>, iter_value_t<I2> &>,
            invoke_result_t<F &, iter_reference_t<I1>, iter_reference_t<I2>>)
    );

    template<typename F, typename I1, typename I2>
    CPP_concept_bool indirectly_regular_binary_invocable_ =
        readable<I1> && readable<I2> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirectly_regular_binary_invocable_frag_, F, I1, I2);
    /// \endcond

    template<typename F, typename I>
    CPP_concept_fragment(indirect_unary_predicate_, (F, I),
        predicate<F &, iter_value_t<I> &> &&
        predicate<F &, iter_reference_t<I>> &&
        predicate<F &, iter_common_reference_t<I>>
    );

    template<typename F, typename I>
    CPP_concept_bool indirect_unary_predicate =
        readable<I> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirect_unary_predicate_, F, I);

    template<typename F, typename I1, typename I2>
    CPP_concept_fragment(indirect_binary_predicate_frag_, (F, I1, I2),
        predicate<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
        predicate<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
        predicate<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
        predicate<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
        predicate<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    template<typename F, typename I1, typename I2>
    CPP_concept_bool indirect_binary_predicate_ =
        readable<I1> && readable<I2> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirect_binary_predicate_frag_, F, I1, I2);

    template<typename F, typename I1, typename I2>
    CPP_concept_fragment(indirect_relation_, (F, I1, I2),
        relation<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
        relation<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
        relation<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
        relation<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
        relation<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    template<typename F, typename I1, typename I2 = I1>
    CPP_concept_bool indirect_relation =
        readable<I1> && readable<I2> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirect_relation_, F, I1, I2);

    template<typename F, typename I1, typename I2>
    CPP_concept_fragment(indirect_strict_weak_order_, (F, I1, I2),
        strict_weak_order<F &, iter_value_t<I1> &, iter_value_t<I2> &> &&
        strict_weak_order<F &, iter_value_t<I1> &, iter_reference_t<I2>> &&
        strict_weak_order<F &, iter_reference_t<I1>, iter_value_t<I2> &> &&
        strict_weak_order<F &, iter_reference_t<I1>, iter_reference_t<I2>> &&
        strict_weak_order<F &, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
    );

    template<typename F, typename I1, typename I2 = I1>
    CPP_concept_bool indirect_strict_weak_order =
        readable<I1> && readable<I2> &&
        copy_constructible<F> &&
        CPP_fragment(ranges::indirect_strict_weak_order_, F, I1, I2);
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
    template<typename I, typename O>
    CPP_concept_fragment(indirectly_movable_, (I, O),
        writable<O, iter_rvalue_reference_t<I>>
    );

    template<typename I, typename O>
    CPP_concept_bool indirectly_movable =
        readable<I> && CPP_fragment(ranges::indirectly_movable_, I, O);

    template<typename I, typename O>
    CPP_concept_fragment(indirectly_movable_storable_, (I, O),
        writable<O, iter_value_t<I>> &&
        movable<iter_value_t<I>> &&
        constructible_from<iter_value_t<I>, iter_rvalue_reference_t<I>> &&
        assignable_from<iter_value_t<I> &, iter_rvalue_reference_t<I>>
    );

    template<typename I, typename O>
    CPP_concept_bool indirectly_movable_storable =
        indirectly_movable<I, O> &&
        CPP_fragment(ranges::indirectly_movable_storable_, I, O);

    template<typename I, typename O>
    CPP_concept_fragment(indirectly_copyable_, (I, O),
        writable<O, iter_reference_t<I>>
    );

    template<typename I, typename O>
    CPP_concept_bool indirectly_copyable =
        readable<I> && CPP_fragment(ranges::indirectly_copyable_, I, O);

    template<typename I, typename O>
    CPP_concept_fragment(indirectly_copyable_storable_, (I, O),
        writable<O, iter_value_t<I> const &> &&
        copyable<iter_value_t<I>> &&
        constructible_from<iter_value_t<I>, iter_reference_t<I>> &&
        assignable_from<iter_value_t<I> &, iter_reference_t<I>>
    );

    template<typename I, typename O>
    CPP_concept_bool indirectly_copyable_storable =
        indirectly_copyable<I, O> &&
        CPP_fragment(ranges::indirectly_copyable_storable_, I, O);

    template<typename I1, typename I2 = I1>
    CPP_concept_bool indirectly_swappable =
        readable<I1> && readable<I2> &&
        CPP_requires ((I1 const) i1, (I2 const) i2) //
        (
            ranges::iter_swap(i1, i2),
            ranges::iter_swap(i1, i1),
            ranges::iter_swap(i2, i2),
            ranges::iter_swap(i2, i1)
        );

    template<typename C, typename I1, typename P1, typename I2, typename P2>
    CPP_concept_fragment(projected_indirect_relation_, (C, I1, P1, I2, P2),
        indirect_relation<C, projected<I1, P1>, projected<I2, P2>>
    );

    template<typename I1, typename I2, typename C, typename P1 = identity,
        typename P2 = identity>
    CPP_concept_bool indirectly_comparable =
        CPP_fragment(ranges::projected_indirect_relation_, C, I1, P1, I2, P2);

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Composite concepts for use defining algorithms:
    template<typename I>
    CPP_concept_bool permutable =
        forward_iterator<I> &&
        indirectly_swappable<I, I> &&
        indirectly_movable_storable<I, I>;

    template<typename C, typename I1, typename P1, typename I2, typename P2>
    CPP_concept_fragment(projected_indirect_strict_weak_order_, (C, I1, P1, I2, P2),
        indirect_strict_weak_order<C, projected<I1, P1>, projected<I2, P2>>
    );

    template<typename I1, typename I2, typename Out, typename C = less,
        typename P1 = identity, typename P2 = identity>
    CPP_concept_bool mergeable =
        input_iterator<I1> &&
        input_iterator<I2> &&
        weakly_incrementable<Out> &&
        indirectly_copyable<I1, Out> &&
        indirectly_copyable<I2, Out> &&
        CPP_fragment(ranges::projected_indirect_strict_weak_order_, C, I1, P1, I2, P2);

    template<typename I, typename C = less, typename P = identity>
    CPP_concept_bool sortable =
        permutable<I> &&
        CPP_fragment(ranges::projected_indirect_strict_weak_order_, C, I, P, I, P);
    // clang-format on

    struct sentinel_tag
    {};
    struct sized_sentinel_tag : sentinel_tag
    {};

    template<typename S, typename I>
    using sentinel_tag_of =               //
        std::enable_if_t<                 //
            sentinel_for<S, I>,           //
            detail::if_then_t<            //
                sized_sentinel_for<S, I>, //
                sized_sentinel_tag,       //
                sentinel_tag>>;

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
        using ranges::sized_sentinel_for;
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
} // namespace ranges

#endif // defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 3900)

#include <range/v3/detail/reenable_warnings.hpp>

#endif // RANGES_V3_ITERATOR_CONCEPTS_HPP
