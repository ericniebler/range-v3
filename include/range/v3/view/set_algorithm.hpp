/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
            // indirect_move is put here instead of in set_??_view::cursor to
            // work around gcc friend name injection bug.
            template<typename Cursor>
            struct set_op_cursor_move
            {
                template<typename Sent>
                friend auto indirect_move(basic_iterator<Cursor, Sent> const &it)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    get_cursor(it).indirect_move_()
                )
            };
            
            template<typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2,
                     template<bool, typename...> class Cursor, cardinality Cardinality>
            struct set_algorithm_view
            : view_facade<
                    set_algorithm_view<Rng1, Rng2, C, P1, P2, Cursor, Cardinality>,
                    Cardinality>
            {
            private:
                friend range_access;
                semiregular_t<function_type<C>> pred_;
                semiregular_t<function_type<P1>> proj1_;
                semiregular_t<function_type<P2>> proj2_;
                Rng1 rng1_;
                Rng2 rng2_;

                struct sentinel;
                template<bool IsConst>
                using cursor = Cursor<IsConst, Rng1, Rng2, C, P1, P2>;

                cursor<false> begin_cursor()
                {
                    return {pred_, proj1_, proj2_,
                            ranges::begin(rng1_), ranges::end(rng1_),
                            ranges::begin(rng2_), ranges::end(rng2_)};
                }
                CONCEPT_REQUIRES(Range<Rng1 const>() && Range<Rng2 const>())
                cursor<true> begin_cursor() const
                {
                    return {pred_, proj1_, proj2_,
                            ranges::begin(rng1_), ranges::end(rng1_),
                            ranges::begin(rng2_), ranges::end(rng2_)};
                }
            public:
                set_algorithm_view() = default;
                set_algorithm_view(Rng1 rng1, Rng2 rng2,
                                   C pred, P1 proj1, P2 proj2)
                : pred_(as_function(std::move(pred)))
                , proj1_(as_function(std::move(proj1)))
                , proj2_(as_function(std::move(proj2)))
                , rng1_(std::move(rng1))
                , rng2_(std::move(rng2))
                {}
            };

            
            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_difference_cursor
                : detail::set_op_cursor_move<set_difference_cursor<IsConst, Rng1, Rng2, C, P1, P2>>
            {
            private:
                using pred_ref_ = semiregular_ref_or_val_t<function_type<C>, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<function_type<P1>, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<function_type<P2>, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;
                
                template<typename T>
                using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
                
                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;
                
                range_iterator_t<R1> it1_;
                range_sentinel_t<R1> end1_;
                
                range_iterator_t<R2> it2_;
                range_sentinel_t<R2> end2_;
            
                void satisfy()
                {
                    while(it1_ != end1_)
                    {
                        if(it2_ == end2_)
                            return;

                        if(pred_(proj1_(*it1_), proj2_(*it2_)))
                            return;
                        
                        if(!pred_(proj2_(*it2_), proj1_(*it1_)))
                            ++it1_;
                        
                        ++it2_;
                    }
                }
                
            public:
                using value_type = range_value_t<constify_if<Rng1>>;

                set_difference_cursor() = default;
                set_difference_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                      range_iterator_t<R1> it1, range_sentinel_t<R1> end1,
                                      range_iterator_t<R2> it2, range_sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2))
                {
                    satisfy();
                }
                auto get() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    *it1_
                )
                void next()
                {
                    ++it1_;
                    satisfy();
                }
                bool equal(set_difference_cursor const &that) const
                {
                    return (it1_ == that.it1_ && it2_ == that.it2_) ||
                           (done() && that.done());
                }
                bool done() const
                {
                    return it1_ == end1_;
                }
                auto indirect_move_() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(it1_)
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

        
        template<typename Rng1, typename Rng2,
                 typename C, typename P1, typename P2>
        using set_difference_view = detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                 detail::set_difference_cursor,
                 detail::set_difference_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;
        
        namespace view
        {
            struct set_difference_fn
            {
            public:
                template<typename Rng1, typename Rng2,
                         typename C, typename P1, typename P2,
                         typename I1 = range_iterator_t<Rng1>,
                         typename I2 = range_iterator_t<Rng2>>
                using Concept = meta::and_<
                    InputRange<Rng1>, InputRange<Rng2>,
                    IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>
                >;
                template<typename Rng1, typename Rng2,
                    typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                    CONCEPT_REQUIRES_(Concept<Rng1, Rng2, C, P1, P2>())>
                set_difference_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>
                operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
                {
                    return {all(std::forward<Rng1>(rng1)),
                            all(std::forward<Rng2>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }


            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2,
                    typename C, typename P1, typename P2,
                    typename I1 = range_iterator_t<Rng1>,
                    typename I2 = range_iterator_t<Rng2>,
                    CONCEPT_REQUIRES_(!Concept<Rng1, Rng2, C, P1, P2>())>
                void operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred, P1 proj1, P2 proj2) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng1>(),
                        "The first parameter of view::set_difference "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<Rng2>(),
                        "The second parameter of view::set_difference "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>(),
                        "The predicate function passed to view::set_difference "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P1, range_value_t<Rng1>>(),
                        "The first projection function passed to view::set_difference "
                        "must be callable with an argument of the first range's value type.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P2, range_value_t<Rng2>>(),
                        "The second projection function passed to view::set_difference "
                        "must be callable with an argument of the second range's value type.");
                }

            #endif
            };

            /// \relates set_difference_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& set_difference = static_const<view<set_difference_fn>>::value;
            }
        }
        /// @}
        
        
        
        
        namespace detail
        {
            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_intersection_cursor
                : detail::set_op_cursor_move<set_intersection_cursor<IsConst, Rng1, Rng2, C, P1, P2>>
            {
            private:
                using pred_ref_ = semiregular_ref_or_val_t<function_type<C>, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<function_type<P1>, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<function_type<P2>, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;
                
                template<typename T>
                using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
                
                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;

                range_iterator_t<R1> it1_;
                range_sentinel_t<R1> end1_;
                
                range_iterator_t<R2> it2_;
                range_sentinel_t<R2> end2_;
                
                void satisfy()
                {
                    while(it1_ != end1_ && it2_ != end2_)
                    {
                        if(pred_(proj1_(*it1_), proj2_(*it2_)))
                            ++it1_;
                        else
                        {
                            if(!pred_(proj2_(*it2_), proj1_(*it1_)))
                                return;

                            ++it2_;
                        }
                    }
                }
                
            public:
                using value_type = range_value_t<R1>;

                set_intersection_cursor() = default;
                set_intersection_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                        range_iterator_t<R1> it1, range_sentinel_t<R1> end1,
                                        range_iterator_t<R2> it2, range_sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2))
                {
                    satisfy();
                }
                auto get() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    *it1_
                )
                void next()
                {
                    ++it1_;
                    ++it2_;
                    satisfy();        
                }
                bool equal(set_intersection_cursor const &that) const
                {
                    return (it1_ == that.it1_ && it2_ == that.it2_) ||
                           (done() && that.done());
                }
                bool done() const
                {
                    return (it1_ == end1_) || (it2_ == end2_);
                }
                auto indirect_move_() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(it1_)
                )
            };
        
            constexpr cardinality set_intersection_cardinality(cardinality c1, cardinality c2)
            {
                return (c1 == unknown) || (c2 == unknown) ? unknown :
                       (c1 >= 0 || c1 == finite) || (c2 >= 0 || c2 == finite) ? finite : unknown;
            }
            
        }
        /// \endcond
        
        
        template<typename Rng1, typename Rng2,
                 typename C, typename P1, typename P2>
        using set_intersection_view = detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                 detail::set_intersection_cursor,
                 detail::set_intersection_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;
        
        namespace view
        {
            struct set_intersection_fn
            {
            public:
                template<typename Rng1, typename Rng2,
                         typename C, typename P1, typename P2,
                         typename I1 = range_iterator_t<Rng1>,
                         typename I2 = range_iterator_t<Rng2>>
                using Concept = meta::and_<
                    InputRange<Rng1>, InputRange<Rng2>,
                    IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>
                >;
                template<typename Rng1, typename Rng2,
                    typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                    CONCEPT_REQUIRES_(Concept<Rng1, Rng2, C, P1, P2>())>
                set_intersection_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>
                operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
                {
                    return {all(std::forward<Rng1>(rng1)),
                            all(std::forward<Rng2>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }


            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2,
                    typename C, typename P1, typename P2,
                    typename I1 = range_iterator_t<Rng1>,
                    typename I2 = range_iterator_t<Rng2>,
                    CONCEPT_REQUIRES_(!Concept<Rng1, Rng2, C, P1, P2>())>
                void operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred, P1 proj1, P2 proj2) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng1>(),
                        "The first parameter of view::set_intersection "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<Rng2>(),
                        "The second parameter of view::set_intersection "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>(),
                        "The predicate function passed to view::set_intersection "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P1, range_value_t<Rng1>>(),
                        "The first projection function passed to view::set_intersection "
                        "must be callable with an argument of the first range's value type.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P2, range_value_t<Rng2>>(),
                        "The second projection function passed to view::set_intersection "
                        "must be callable with an argument of the second range's value type.");
                }

            #endif
            };

            /// \relates set_intersection_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& set_intersection = static_const<view<set_intersection_fn>>::value;
            }
        }
        /// @}
        
        
        
        namespace detail
        {
            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_union_cursor
                : detail::set_op_cursor_move<set_union_cursor<IsConst, Rng1, Rng2, C, P1, P2>>
            {
            private:
                using pred_ref_ = semiregular_ref_or_val_t<function_type<C>, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<function_type<P1>, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<function_type<P2>, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;
                
                template<typename T>
                using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
                
                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;
                
                range_iterator_t<R1> it1_;
                range_sentinel_t<R1> end1_;
                
                range_iterator_t<R2> it2_;
                range_sentinel_t<R2> end2_;
                
                enum class state_t
                {
                    FIRST, FIRST_INC_SECOND, SECOND, ONLY_FIRST, ONLY_SECOND
                } state;
                
                state_t which_set() const
                {
                    if(it1_ != end1_)
                    {
                        if(it2_ == end2_)
                            return state_t::ONLY_FIRST;
                        
                        if(pred_(proj2_(*it2_), proj1_(*it1_)))
                            return state_t::SECOND;
                        
                        // take care of the case when iter_move is made from it1_
                        // so we need to test in advance when we still have access to unmoved-from *it1_
                        if(!pred_(proj1_(*it1_), proj2_(*it2_)))
                             return state_t::FIRST_INC_SECOND;

                        return state_t::FIRST;
                    }
                    
                    return state_t::ONLY_SECOND;
                }
                
            public:
                using value_type = common_type_t<range_value_t<R1>, range_value_t<R2>>;
                using reference_type = common_reference_t<range_reference_t<R1>, range_reference_t<R2>>;
                using rvalue_reference_type = common_reference_t<range_rvalue_reference_t<R1>,
                                                                 range_rvalue_reference_t<R2>>;
                using single_pass = meta::fast_or<SinglePass<range_iterator_t<R1>>,
                                                  SinglePass<range_iterator_t<R2>>>;

                set_union_cursor() = default;
                set_union_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                 range_iterator_t<R1> it1, range_sentinel_t<R1> end1,
                                 range_iterator_t<R2> it2, range_sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2)),
                    state(which_set())
                {}
                reference_type get() const
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
                            state = which_set();
                            break;
                            
                        case state_t::FIRST_INC_SECOND:
                            ++it2_;
                            ++it1_;
                            state = which_set();
                            break;
                        
                        case state_t::ONLY_FIRST:
                            ++it1_;
                            break;
                            
                        case state_t::SECOND:
                            ++it2_;
                            state = which_set();
                            break;
                            
                        case state_t::ONLY_SECOND:
                            ++it2_;
                            break;
                    }
                }
                bool equal(set_union_cursor const &that) const
                {
                    return (it1_ == that.it1_ && it2_ == that.it2_) ||
                           (done() && that.done());
                }
                bool done() const
                {
                    return (it1_ == end1_) && (it2_ == end2_);
                }
                rvalue_reference_type indirect_move_() const
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

        
        template<typename Rng1, typename Rng2,
                 typename C, typename P1, typename P2>
        using set_union_view = detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                 detail::set_union_cursor,
                 detail::set_union_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;
        
        namespace view
        {
            struct set_union_fn
            {
            public:
                template<typename Rng1, typename Rng2,
                         typename C, typename P1, typename P2,
                         typename I1 = range_iterator_t<Rng1>,
                         typename I2 = range_iterator_t<Rng2>>
                using Concept = meta::and_<
                    InputRange<Rng1>, InputRange<Rng2>,
                    Common<range_value_t<Rng1>, range_value_t<Rng2>>,
                    CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>>,
                    CommonReference<range_rvalue_reference_t<Rng1>, range_rvalue_reference_t<Rng2>>,
                    IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>
                >;
                template<typename Rng1, typename Rng2,
                    typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                    CONCEPT_REQUIRES_(Concept<Rng1, Rng2, C, P1, P2>())>
                set_union_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>
                operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
                {
                    return {all(std::forward<Rng1>(rng1)),
                            all(std::forward<Rng2>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }


            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2,
                    typename C, typename P1, typename P2,
                    typename I1 = range_iterator_t<Rng1>,
                    typename I2 = range_iterator_t<Rng2>,
                    CONCEPT_REQUIRES_(!Concept<Rng1, Rng2, C, P1, P2>())>
                void operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred, P1 proj1, P2 proj2) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng1>(),
                        "The first parameter of view::set_union "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<Rng2>(),
                        "The second parameter of view::set_union "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(Common<range_value_t<Rng1>, range_value_t<Rng2>>(),
                        "The value types of the two ranges must share a common type.");
                    CONCEPT_ASSERT_MSG(CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>>(),
                        "The reference types of the two ranges must share a common reference.");
                    CONCEPT_ASSERT_MSG(CommonReference<range_rvalue_reference_t<Rng1>, range_rvalue_reference_t<Rng2>>(),
                        "The rvalue reference types of the two ranges must share a common reference.");
                    CONCEPT_ASSERT_MSG(
                        IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>(),
                        "The predicate function passed to view::set_union "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P1, range_value_t<Rng1>>(),
                        "The first projection function passed to view::set_union "
                        "must be callable with an argument of the first range's value type.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P2, range_value_t<Rng2>>(),
                        "The second projection function passed to view::set_union "
                        "must be callable with an argument of the second range's value type.");
                }

            #endif
            };

            /// \relates set_union_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& set_union = static_const<view<set_union_fn>>::value;
            }
        }
        /// @}
        
        
        
        namespace detail
        {
            template<bool IsConst,
                     typename Rng1, typename Rng2,
                     typename C, typename P1, typename P2>
            struct set_symmetric_difference_cursor
                : detail::set_op_cursor_move<set_symmetric_difference_cursor<IsConst, Rng1, Rng2, C, P1, P2>>
            {
            private:
                using pred_ref_ = semiregular_ref_or_val_t<function_type<C>, IsConst>;
                using proj1_ref_ = semiregular_ref_or_val_t<function_type<P1>, IsConst>;
                using proj2_ref_ = semiregular_ref_or_val_t<function_type<P2>, IsConst>;
                pred_ref_ pred_;
                proj1_ref_ proj1_;
                proj2_ref_ proj2_;
                
                template<typename T>
                using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
                
                using R1 = constify_if<Rng1>;
                using R2 = constify_if<Rng2>;

                
                range_iterator_t<R1> it1_;
                range_sentinel_t<R1> end1_;
                
                range_iterator_t<R2> it2_;
                range_sentinel_t<R2> end2_;
                
                enum class state_t
                {
                    FIRST, SECOND, ONLY_FIRST, ONLY_SECOND
                } state;
                
                void satisfy()
                {
                    while(it1_ != end1_)
                    {
                        if(it2_ == end2_)
                        {
                            state = state_t::ONLY_FIRST;
                            return;
                        }
                        
                        if(pred_(proj1_(*it1_), proj2_(*it2_)))
                        {
                            state = state_t::FIRST;
                            return;
                        }
                        else
                        {
                            if(pred_(proj2_(*it2_), proj1_(*it1_)))
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
                using single_pass = meta::fast_or<SinglePass<range_iterator_t<R1>>, SinglePass<range_iterator_t<R2>>>;

                set_symmetric_difference_cursor() = default;
                set_symmetric_difference_cursor(pred_ref_ pred, proj1_ref_ proj1, proj2_ref_ proj2,
                                                range_iterator_t<R1> it1, range_sentinel_t<R1> end1,
                                                range_iterator_t<R2> it2, range_sentinel_t<R2> end2)
                  : pred_(std::move(pred)), proj1_(std::move(proj1)), proj2_(std::move(proj2)),
                    it1_(std::move(it1)), end1_(std::move(end1)), it2_(std::move(it2)), end2_(std::move(end2)),
                    state()
                {
                    satisfy();
                }
                reference_type get() const
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
                bool equal(set_symmetric_difference_cursor const &that) const
                {
                    return (it1_ == that.it1_ || it2_ == that.it2_) ||
                           (done() && that.done());
                }
                bool done() const
                {
                    return (it1_ == end1_) && (it2_ == end2_);
                }
                rvalue_reference_type indirect_move_() const
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

        
        template<typename Rng1, typename Rng2,
                 typename C, typename P1, typename P2>
        using set_symmetric_difference_view = detail::set_algorithm_view<Rng1, Rng2, C, P1, P2,
                 detail::set_symmetric_difference_cursor,
                 detail::set_symmetric_difference_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>;
        
        namespace view
        {
            struct set_symmetric_difference_fn
            {
            public:
                template<typename Rng1, typename Rng2,
                         typename C, typename P1, typename P2,
                         typename I1 = range_iterator_t<Rng1>,
                         typename I2 = range_iterator_t<Rng2>>
                using Concept = meta::and_<
                    InputRange<Rng1>, InputRange<Rng2>,
                    Common<range_value_t<Rng1>, range_value_t<Rng2>>,
                    CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>>,
                    CommonReference<range_rvalue_reference_t<Rng1>, range_rvalue_reference_t<Rng2>>,
                    IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>
                >;
                template<typename Rng1, typename Rng2,
                    typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                    CONCEPT_REQUIRES_(Concept<Rng1, Rng2, C, P1, P2>())>
                set_symmetric_difference_view<all_t<Rng1>, all_t<Rng2>, C, P1, P2>
                operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
                {
                    return {all(std::forward<Rng1>(rng1)),
                            all(std::forward<Rng2>(rng2)),
                            std::move(pred),
                            std::move(proj1),
                            std::move(proj2)};
                }


            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng1, typename Rng2,
                    typename C, typename P1, typename P2,
                    typename I1 = range_iterator_t<Rng1>,
                    typename I2 = range_iterator_t<Rng2>,
                    CONCEPT_REQUIRES_(!Concept<Rng1, Rng2, C, P1, P2>())>
                void operator()(Rng1 && rng1, Rng2 && rng2,
                    C pred, P1 proj1, P2 proj2) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng1>(),
                        "The first parameter of view::set_symmetric_difference "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<Rng2>(),
                        "The second parameter of view::set_symmetric_difference "
                        "must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(Common<range_value_t<Rng1>, range_value_t<Rng2>>(),
                        "The value types of the two ranges must share a common type.");
                    CONCEPT_ASSERT_MSG(CommonReference<range_reference_t<Rng1>, range_reference_t<Rng2>>(),
                        "The reference types of the two ranges must share a common reference.");
                    CONCEPT_ASSERT_MSG(CommonReference<range_rvalue_reference_t<Rng1>, range_rvalue_reference_t<Rng2>>(),
                        "The rvalue reference types of the two ranges must share a common reference.");
                    CONCEPT_ASSERT_MSG(
                        IndirectCallableRelation<C, Projected<I1, P1>, Projected<I2, P2>>(),
                        "The predicate function passed to view::set_symmetric_difference "
                        "must be callable with two arguments of the two "
                        "input ranges' value types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P1, range_value_t<Rng1>>(),
                        "The first projection function passed to view::set_symmetric_difference "
                        "must be callable with an argument of the first range's value type.");
                    CONCEPT_ASSERT_MSG(
                        Callable<P2, range_value_t<Rng2>>(),
                        "The second projection function passed to view::set_symmetric_difference "
                        "must be callable with an argument of the second range's value type.");
                }

            #endif
            };

            /// \relates set_symmetric_difference_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& set_symmetric_difference = static_const<view<set_symmetric_difference_fn>>::value;
            }
        }
        /// @}
        
    }
}

#endif
