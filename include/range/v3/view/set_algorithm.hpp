/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Tomislav Ivek 2015-2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_SET_ALGORITHM_HPP
#define RANGES_V3_VIEW_SET_ALGORITHM_HPP

#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng1, typename Rng2, typename C, typename P1, typename P2,
                     template<bool, typename...> class Cursor, cardinality Cardinality>
            struct set_algorithm_view
              : view_facade<
                    set_algorithm_view<Rng1, Rng2, C, P1, P2, Cursor, Cardinality>,
                    Cardinality>
            {
            private:
                friend range_access;
                semiregular_t<C> pred_;
                semiregular_t<P1> proj1_;
                semiregular_t<P2> proj2_;
                Rng1 rng1_;
                Rng2 rng2_;

                template<bool IsConst>
                using cursor = Cursor<IsConst, Rng1, Rng2, C, P1, P2>;

                cursor<simple_view<Rng1>() && simple_view<Rng2>()> begin_cursor()
                {
                    return {pred_, proj1_, proj2_,
                            ranges::begin(rng1_), ranges::end(rng1_),
                            ranges::begin(rng2_), ranges::end(rng2_)};
                }
                CPP_member
                auto begin_cursor() const -> CPP_ret(cursor<true>)(
                    requires Range<Rng1 const> && Range<Rng2 const>)
                {
                    return {pred_, proj1_, proj2_,
                            ranges::begin(rng1_), ranges::end(rng1_),
                            ranges::begin(rng2_), ranges::end(rng2_)};
                }
            public:
                set_algorithm_view() = default;
                set_algorithm_view(Rng1 rng1, Rng2 rng2, C pred, P1 proj1, P2 proj2)
                  : pred_(std::move(pred))
                  , proj1_(std::move(proj1))
                  , proj2_(std::move(proj2))
                  , rng1_(std::move(rng1))
                  , rng2_(std::move(rng2))
                {}
            };

            template<bool IsConst, typename Rng1, typename Rng2, typename C, typename P1,
                typename P2>
            struct set_difference_cursor
            {
            private:
                friend struct set_difference_cursor<!IsConst, Rng1, Rng2, C, P1, P2>;
                using pred_ref_ = semiregular_ref_or_val_t<C, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<P1, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<P2, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;

                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;

                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;

                iterator_t<R1> it1_;
                sentinel_t<R1> end1_;

                iterator_t<R2> it2_;
                sentinel_t<R2> end2_;

                void satisfy()
                {
                    while(it1_ != end1_)
                    {
                        if(it2_ == end2_)
                            return;

                        if(invoke(pred_, invoke(proj1_, *it1_), invoke(proj2_, *it2_)))
                            return;

                        if(!invoke(pred_, invoke(proj2_, *it2_), invoke(proj1_, *it1_)))
                            ++it1_;

                        ++it2_;
                    }
                }

            public:
                using value_type = range_value_t<constify_if<Rng1>>;
                using single_pass = meta::or_c<SinglePass<iterator_t<R1>>,
                                               SinglePass<iterator_t<R2>>>;

                set_difference_cursor() = default;
                set_difference_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                      iterator_t<R1> it1, sentinel_t<R1> end1,
                                      iterator_t<R2> it2, sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2))
                {
                    satisfy();
                }
                template<bool Other>
                CPP_ctor(set_difference_cursor)(set_difference_cursor<Other, Rng1, Rng2, C, P1, P2> that)(
                    requires IsConst && !Other)
                  : pred_(std::move(that.pred_)), proj1_(std::move(that.proj1_))
                  , proj2_(std::move(that.proj2_)), it1_(std::move(that.it1_))
                  , end1_(std::move(that.end1_)), it2_(std::move(that.it2_))
                  , end2_(std::move(that.end2_))
                {}
                auto CPP_auto_fun(read)() (const)
                (
                    return *it1_
                )
                void next()
                {
                    ++it1_;
                    satisfy();
                }
                CPP_member
                auto equal(set_difference_cursor const &that) const ->
                    CPP_ret(bool)(
                        requires ForwardRange<Rng1>)
                {
                    return it1_ == that.it1_; // does not support comparing iterators from different ranges
                }
                bool equal(default_sentinel) const
                {
                    return it1_ == end1_;
                }
                auto CPP_auto_fun(move)() (const)
                (
                    return iter_move(it1_)
                )
            };

            constexpr cardinality set_difference_cardinality(cardinality c1, cardinality c2)
            {
                return (c1 == unknown) ? unknown :
                    (c1 >= 0) || (c1 == finite) ? finite : // else, c1 == infinite
                        (c2 >= 0) || (c2 == finite) ? infinite : unknown;
            }
        }
        /// \endcond

        template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
        using set_difference_view =
            detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                detail::set_difference_cursor,
                detail::set_difference_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;

        namespace view
        {
            CPP_def
            (
                template(typename Rng1, typename Rng2, typename C, typename P1, typename P2)
                concept SetDifferenceViewConcept,
                    InputRange<Rng1> &&
                    InputRange<Rng2> &&
                    IndirectRelation<C, projected<iterator_t<Rng1>, P1>,
                                        projected<iterator_t<Rng2>, P2>>
            );

            struct set_difference_fn
            {
            public:
                template<typename Rng1, typename Rng2, typename C = ordered_less,
                    typename P1 = ident, typename P2 = ident>
                auto operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                        P2 proj2 = P2{}) const ->
                    CPP_ret(set_difference_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>)(
                        requires SetDifferenceViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    return {all(static_cast<Rng1 &&>(rng1)),
                            all(static_cast<Rng2 &&>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
                auto operator()(Rng1 &&, Rng2 &&, C, P1, P2) const ->
                    CPP_ret(void)(
                        requires not SetDifferenceViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    using I1 = iterator_t<Rng1>;
                    using I2 = iterator_t<Rng2>;
                    CPP_assert_msg(InputRange<Rng1>,
                        "The first parameter of view::set_difference "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(InputRange<Rng2>,
                        "The second parameter of view::set_difference "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(
                        IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>,
                        "The predicate function passed to view::set_difference "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CPP_assert_msg(
                        Invocable<P1&, range_value_t<Rng1>>,
                        "The first projection function passed to view::set_difference "
                        "must be callable with an argument of the first range's value type.");
                    CPP_assert_msg(
                        Invocable<P2&, range_value_t<Rng2>>,
                        "The second projection function passed to view::set_difference "
                        "must be callable with an argument of the second range's value type.");
                }
            #endif
            };

            /// \relates set_difference_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<set_difference_fn>, set_difference)
        }
        /// @}

        namespace detail
        {
            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_intersection_cursor
            {
            private:
                friend struct set_intersection_cursor<!IsConst, Rng1, Rng2, C, P1, P2>;
                using pred_ref_ = semiregular_ref_or_val_t<C, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<P1, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<P2, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;

                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;

                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;

                iterator_t<R1> it1_;
                sentinel_t<R1> end1_;

                iterator_t<R2> it2_;
                sentinel_t<R2> end2_;

                void satisfy()
                {
                    while(it1_ != end1_ && it2_ != end2_)
                    {
                        if(invoke(pred_, invoke(proj1_, *it1_), invoke(proj2_, *it2_)))
                            ++it1_;
                        else
                        {
                            if(!invoke(pred_, invoke(proj2_, *it2_), invoke(proj1_, *it1_)))
                                return;
                            ++it2_;
                        }
                    }
                }

            public:
                using value_type = range_value_t<R1>;
                using single_pass = meta::or_c<SinglePass<iterator_t<R1>>,
                                               SinglePass<iterator_t<R2>>>;

                set_intersection_cursor() = default;
                set_intersection_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                        iterator_t<R1> it1, sentinel_t<R1> end1,
                                        iterator_t<R2> it2, sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2))
                {
                    satisfy();
                }
                template<bool Other>
                CPP_ctor(set_intersection_cursor)(set_intersection_cursor<Other, Rng1, Rng2, C, P1, P2> that)(
                    requires IsConst && !Other)
                  : pred_(std::move(that.pred_)), proj1_(std::move(that.proj1_))
                  , proj2_(std::move(that.proj2_)), it1_(std::move(that.it1_))
                  , end1_(std::move(that.end1_)), it2_(std::move(that.it2_))
                  , end2_(std::move(that.end2_))
                {}
                auto CPP_auto_fun(read)() (const)
                (
                    return *it1_
                )
                void next()
                {
                    ++it1_;
                    ++it2_;
                    satisfy();
                }
                CPP_member
                auto equal(set_intersection_cursor const &that) const ->
                    CPP_ret(bool)(
                        requires ForwardRange<Rng1>)
                {
                    return it1_ == that.it1_; // does not support comparing iterators from different ranges;
                }
                bool equal(default_sentinel) const
                {
                    return (it1_ == end1_) || (it2_ == end2_);
                }
                auto CPP_auto_fun(move)() (const)
                (
                    return iter_move(it1_)
                )
            };

            constexpr cardinality set_intersection_cardinality(cardinality c1, cardinality c2)
            {
                return (c1 == unknown) || (c2 == unknown) ? unknown :
                       (c1 >= 0 || c1 == finite) || (c2 >= 0 || c2 == finite) ? finite : unknown;
            }
        }
        /// \endcond

        template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
        using set_intersection_view =
            detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                detail::set_intersection_cursor,
                detail::set_intersection_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;

        namespace view
        {
            CPP_def
            (
                template(typename Rng1, typename Rng2, typename C, typename P1, typename P2)
                concept SetIntersectionViewConcept,
                    InputRange<Rng1> && InputRange<Rng2> &&
                    IndirectRelation<C, projected<iterator_t<Rng1>, P1>,
                                        projected<iterator_t<Rng2>, P2>>
            );

            struct set_intersection_fn
            {
            public:
                template<typename Rng1, typename Rng2, typename C = ordered_less,
                    typename P1 = ident, typename P2 = ident>
                auto operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                        P2 proj2 = P2{}) const ->
                    CPP_ret(set_intersection_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>)(
                        requires SetIntersectionViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    return {all(static_cast<Rng1 &&>(rng1)),
                            all(static_cast<Rng2 &&>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
                auto operator()(Rng1 &&, Rng2 &&, C, P1, P2) const ->
                    CPP_ret(void)(
                        requires not SetIntersectionViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    using I1 = iterator_t<Rng1>;
                    using I2 = iterator_t<Rng2>;
                    CPP_assert_msg(InputRange<Rng1>,
                        "The first parameter of view::set_intersection "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(InputRange<Rng2>,
                        "The second parameter of view::set_intersection "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(
                        IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>,
                        "The predicate function passed to view::set_intersection "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CPP_assert_msg(
                        Invocable<P1&, range_value_t<Rng1>>,
                        "The first projection function passed to view::set_intersection "
                        "must be callable with an argument of the first range's value type.");
                    CPP_assert_msg(
                        Invocable<P2&, range_value_t<Rng2>>,
                        "The second projection function passed to view::set_intersection "
                        "must be callable with an argument of the second range's value type.");
                }
            #endif
            };

            /// \relates set_intersection_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<set_intersection_fn>, set_intersection)
        }
        /// @}

        namespace detail
        {
            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_union_cursor
            {
            private:
                friend struct set_union_cursor<!IsConst, Rng1, Rng2, C, P1, P2>;
                using pred_ref_ = semiregular_ref_or_val_t<C, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<P1, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<P2, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;

                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;

                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;

                iterator_t<R1> it1_;
                sentinel_t<R1> end1_;

                iterator_t<R2> it2_;
                sentinel_t<R2> end2_;

                enum class state_t
                {
                    FIRST, SECOND, ONLY_FIRST, ONLY_SECOND
                } state;

                void satisfy()
                {
                    if(it1_ == end1_)
                    {
                        state = state_t::ONLY_SECOND;
                        return;
                    }

                    if(it2_ == end2_)
                    {
                        state = state_t::ONLY_FIRST;
                        return;
                    }

                    if(invoke(pred_, invoke(proj2_, *it2_), invoke(proj1_, *it1_)))
                    {
                        state = state_t::SECOND;
                        return;
                    }

                    if(!invoke(pred_, invoke(proj1_, *it1_), invoke(proj2_, *it2_)))
                        ++it2_;

                    state = state_t::FIRST;
                }

            public:
                using value_type = common_type_t<range_value_t<R1>, range_value_t<R2>>;
                using reference_type = common_reference_t<range_reference_t<R1>, range_reference_t<R2>>;
                using rvalue_reference_type = common_reference_t<range_rvalue_reference_t<R1>,
                                                                 range_rvalue_reference_t<R2>>;
                using single_pass = meta::or_c<SinglePass<iterator_t<R1>>,
                                               SinglePass<iterator_t<R2>>>;

                set_union_cursor() = default;
                set_union_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                 iterator_t<R1> it1, sentinel_t<R1> end1,
                                 iterator_t<R2> it2, sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2))
                {
                    satisfy();
                }
                template<bool Other>
                CPP_ctor(set_union_cursor)(set_union_cursor<Other, Rng1, Rng2, C, P1, P2> that)(
                    requires IsConst && !Other)
                  : pred_(std::move(that.pred_)), proj1_(std::move(that.proj1_))
                  , proj2_(std::move(that.proj2_)), it1_(std::move(that.it1_))
                  , end1_(std::move(that.end1_)), it2_(std::move(that.it2_))
                  , end2_(std::move(that.end2_))
                {}
                reference_type read() const
                noexcept(noexcept(*it1_) && noexcept(*it2_))
                {
                    if(state == state_t::SECOND || state == state_t::ONLY_SECOND)
                        return *it2_;
                    else
                        return *it1_;
                }
                void next()
                {
                    switch(state)
                    {
                        case state_t::FIRST:
                            ++it1_;
                            break;

                        case state_t::ONLY_FIRST:
                            ++it1_;
                            return;

                        case state_t::SECOND:
                            ++it2_;
                            break;

                        case state_t::ONLY_SECOND:
                            ++it2_;
                            return;
                    }
                    satisfy();
                }
                CPP_member
                auto equal(set_union_cursor const &that) const ->
                    CPP_ret(bool)(
                        requires ForwardRange<Rng1> && ForwardRange<Rng2>)
                {
                    return (it1_ == that.it1_) && (it2_ == that.it2_); // does not support comparing iterators from different ranges
                }
                bool equal(default_sentinel) const
                {
                    return (it1_ == end1_) && (it2_ == end2_);
                }
                rvalue_reference_type move() const
                noexcept(noexcept(iter_move(it1_)) && noexcept(iter_move(it2_)))
                {
                    if(state == state_t::SECOND || state == state_t::ONLY_SECOND)
                        return iter_move(it2_);
                    else
                        return iter_move(it1_);
                }
            };

            constexpr cardinality set_union_cardinality(cardinality c1, cardinality c2)
            {
                return (c1 == infinite) || (c2 == infinite) ? infinite :
                    (c1 == unknown) || (c2 == unknown) ? unknown : finite;
            }

        }
        /// \endcond

        template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
        using set_union_view =
            detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                detail::set_union_cursor,
                detail::set_union_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;

        namespace view
        {
            CPP_def
            (
                template(typename Rng1, typename Rng2, typename C, typename P1, typename P2)
                concept SetUnionViewConcept,
                    InputRange<Rng1> && InputRange<Rng2> &&
                    Common<range_value_t<Rng1>, range_value_t<Rng2>> &&
                    CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>> &&
                    CommonReference<range_rvalue_reference_t<Rng1>,
                                    range_rvalue_reference_t<Rng2>> &&
                    IndirectRelation<C, projected<iterator_t<Rng1>, P1>,
                                        projected<iterator_t<Rng2>, P2>>
            );

            struct set_union_fn
            {
            public:
                template<typename Rng1, typename Rng2, typename C = ordered_less,
                    typename P1 = ident, typename P2 = ident>
                auto operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                        P2 proj2 = P2{}) const ->
                    CPP_ret(set_union_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>)(
                        requires SetUnionViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    return {all(static_cast<Rng1 &&>(rng1)),
                            all(static_cast<Rng2 &&>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
                auto operator()(Rng1 &&, Rng2 &&, C, P1, P2) const ->
                    CPP_ret(void)(
                        requires not SetUnionViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    using I1 = iterator_t<Rng1>;
                    using I2 = iterator_t<Rng2>;
                    CPP_assert_msg(InputRange<Rng1>,
                        "The first parameter of view::set_union "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(InputRange<Rng2>,
                        "The second parameter of view::set_union "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(Common<range_value_t<Rng1>, range_value_t<Rng2>>,
                        "The value types of the two ranges must share a common type.");
                    CPP_assert_msg(CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>>,
                        "The reference types of the two ranges must share a common reference.");
                    CPP_assert_msg(CommonReference<range_rvalue_reference_t<Rng1>, range_rvalue_reference_t<Rng2>>,
                        "The rvalue reference types of the two ranges must share a common reference.");
                    CPP_assert_msg(
                        IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>,
                        "The predicate function passed to view::set_union "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CPP_assert_msg(
                        Invocable<P1&, range_value_t<Rng1>>,
                        "The first projection function passed to view::set_union "
                        "must be callable with an argument of the first range's value type.");
                    CPP_assert_msg(
                        Invocable<P2&, range_value_t<Rng2>>,
                        "The second projection function passed to view::set_union "
                        "must be callable with an argument of the second range's value type.");
                }
            #endif
            };

            /// \relates set_union_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<set_union_fn>, set_union)
        }
        /// @}

        namespace detail
        {
            enum class set_symmetric_difference_state_t
            {
                FIRST, SECOND, ONLY_FIRST, ONLY_SECOND
            };

            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_symmetric_difference_cursor
            {
            private:
                friend struct set_symmetric_difference_cursor<!IsConst, Rng1, Rng2, C, P1, P2>;
                using pred_ref_ = semiregular_ref_or_val_t<C, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<P1, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<P2, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;

                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;

                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;

                iterator_t<R1> it1_;
                sentinel_t<R1> end1_;

                iterator_t<R2> it2_;
                sentinel_t<R2> end2_;

                using state_t = set_symmetric_difference_state_t;
                state_t state;

                void satisfy()
                {
                    while(it1_ != end1_)
                    {
                        if(it2_ == end2_)
                        {
                            state = state_t::ONLY_FIRST;
                            return;
                        }

                        if(invoke(pred_, invoke(proj1_, *it1_), invoke(proj2_, *it2_)))
                        {
                            state = state_t::FIRST;
                            return;
                        }
                        else
                        {
                            if(invoke(pred_, invoke(proj2_, *it2_), invoke(proj1_, *it1_)))
                            {
                                state = state_t::SECOND;
                                return;
                            }
                            else
                            {
                                ++it1_;
                                ++it2_;
                            }
                        }
                    }
                    state = state_t::ONLY_SECOND;
                }

            public:
                using value_type = common_type_t<range_value_t<R1>, range_value_t<R2>>;
                using reference_type = common_reference_t<range_reference_t<R1>, range_reference_t<R2>>;
                using rvalue_reference_type = common_reference_t<range_rvalue_reference_t<R1>,
                                                                 range_rvalue_reference_t<R2>>;
                using single_pass = meta::or_c<SinglePass<iterator_t<R1>>,
                                               SinglePass<iterator_t<R2>>>;

                set_symmetric_difference_cursor() = default;
                set_symmetric_difference_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                                iterator_t<R1> it1, sentinel_t<R1> end1,
                                                iterator_t<R2> it2, sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2)),
                    state()
                {
                    satisfy();
                }
                template<bool Other>
                CPP_ctor(set_symmetric_difference_cursor)(
                    set_symmetric_difference_cursor<Other, Rng1, Rng2, C, P1, P2> that)(
                        requires IsConst && !Other)
                  : pred_(std::move(that.pred_)), proj1_(std::move(that.proj1_))
                  , proj2_(std::move(that.proj2_)), it1_(std::move(that.it1_))
                  , end1_(std::move(that.end1_)), it2_(std::move(that.it2_))
                  , end2_(std::move(that.end2_)), state(that.state)
                {}
                reference_type read() const
                noexcept(noexcept(*it1_) && noexcept(*it2_))
                {
                    if(state == state_t::SECOND || state == state_t::ONLY_SECOND)
                        return *it2_;
                    else
                        return *it1_;
                }
                void next()
                {
                    switch(state)
                    {
                        case state_t::FIRST:
                            ++it1_;
                            satisfy();
                            break;
                        case state_t::ONLY_FIRST:
                            ++it1_;
                            break;
                        case state_t::SECOND:
                            ++it2_;
                            satisfy();
                            break;
                        case state_t::ONLY_SECOND:
                            ++it2_;
                            break;
                    }
                }
                CPP_member
                auto equal(set_symmetric_difference_cursor const &that) const ->
                    CPP_ret(bool)(
                        requires ForwardRange<R1> && ForwardRange<R2>)
                {
                    return (it1_ == that.it1_) && (it2_ == that.it2_); // does not support comparing iterators from different ranges
                }
                bool equal(default_sentinel) const
                {
                    return (it1_ == end1_) && (it2_ == end2_);
                }
                rvalue_reference_type move() const
                noexcept(noexcept(iter_move(it1_)) && noexcept(iter_move(it2_)))
                {
                    if(state == state_t::SECOND || state == state_t::ONLY_SECOND)
                        return iter_move(it2_);
                    else
                        return iter_move(it1_);
                }
            };

            constexpr cardinality set_symmetric_difference_cardinality(cardinality c1, cardinality c2)
            {
                return (c1 == unknown) || (c2 == unknown) ? unknown :
                    (c1 == infinite) != (c2 == infinite) ? infinite :
                        (c1 == infinite) && (c2 == infinite) ? unknown : finite;
            }

        }
        /// \endcond

        template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
        using set_symmetric_difference_view =
            detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                detail::set_symmetric_difference_cursor,
                detail::set_symmetric_difference_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;

        namespace view
        {
            CPP_def
            (
                template(typename Rng1, typename Rng2, typename C, typename P1, typename P2)
                concept SetSymmetricDifferenceViewConcept,
                    InputRange<Rng1> && InputRange<Rng2> &&
                    Common<range_value_t<Rng1>, range_value_t<Rng2>> &&
                    CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>> &&
                    CommonReference<range_rvalue_reference_t<Rng1>,
                                    range_rvalue_reference_t<Rng2>> &&
                    IndirectRelation<C, projected<iterator_t<Rng1>, P1>,
                                        projected<iterator_t<Rng2>, P2>>
            );

            struct set_symmetric_difference_fn
            {
            public:
                template<typename Rng1, typename Rng2, typename C = ordered_less,
                    typename P1 = ident, typename P2 = ident>
                auto operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                        P2 proj2 = P2{}) const ->
                    CPP_ret(set_symmetric_difference_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>)(
                        requires SetSymmetricDifferenceViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    return {all(static_cast<Rng1 &&>(rng1)),
                            all(static_cast<Rng2 &&>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2, typename C, typename P1, typename P2>
                auto operator()(Rng1 &&, Rng2 &&, C, P1, P2) const ->
                    CPP_ret(void)(
                        requires not SetSymmetricDifferenceViewConcept<Rng1, Rng2, C, P1, P2>)
                {
                    using I1 = iterator_t<Rng1>;
                    using I2 = iterator_t<Rng2>;
                    CPP_assert_msg(InputRange<Rng1>,
                        "The first parameter of view::set_symmetric_difference "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(InputRange<Rng2>,
                        "The second parameter of view::set_symmetric_difference "
                        "must be a model of the InputRange concept.");
                    CPP_assert_msg(Common<range_value_t<Rng1>, range_value_t<Rng2>>,
                        "The value types of the two ranges must share a common type.");
                    CPP_assert_msg(CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>>,
                        "The reference types of the two ranges must share a common reference.");
                    CPP_assert_msg(CommonReference<range_rvalue_reference_t<Rng1>, range_rvalue_reference_t<Rng2>>,
                        "The rvalue reference types of the two ranges must share a common reference.");
                    CPP_assert_msg(
                        IndirectRelation<C, projected<I1, P1>, projected<I2, P2>>,
                        "The predicate function passed to view::set_symmetric_difference "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CPP_assert_msg(
                        Invocable<P1&, range_value_t<Rng1>>,
                        "The first projection function passed to view::set_symmetric_difference "
                        "must be callable with an argument of the first range's value type.");
                    CPP_assert_msg(
                        Invocable<P2&, range_value_t<Rng2>>,
                        "The second projection function passed to view::set_symmetric_difference "
                        "must be callable with an argument of the second range's value type.");
                }
            #endif
            };

            /// \relates set_symmetric_difference_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<set_symmetric_difference_fn>, set_symmetric_difference)
        }
        /// @}

    }
}

#endif
