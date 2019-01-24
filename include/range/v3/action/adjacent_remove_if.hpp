/// \file
// Range v3 library
//
//  Copyright Eric Niebler
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ACTION_ADJACENT_REMOVE_IF_HPP
#define RANGES_V3_ACTION_ADJACENT_REMOVE_IF_HPP
#include <range/v3/algorithm/adjacent_remove_if.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/utility/static_const.hpp>
#include <utility>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct adjacent_remove_if_fn
            {
            private:
                friend action_access;
                template<typename Pred, typename Proj = ident, CONCEPT_REQUIRES_(!Range<Pred>())>
                static auto bind(adjacent_remove_if_fn adjacent_remove_if, Pred pred, Proj proj = {})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(adjacent_remove_if, std::placeholders::_1, protect(std::move(pred)),
                        protect(std::move(proj)))
                )
            public:
                struct ConceptImpl
                {
                    template<typename R, typename Pred, typename Proj = ident,
                        typename I = iterator_t<R>>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::ForwardRange, R>(),
                            concepts::model_of<concepts::ErasableRange, R, I, I>(),
                            concepts::is_true(IndirectRelation<Pred, projected<I, Proj>>()),
                            concepts::is_true(Permutable<I>())
                        ));
                };
                template<typename R, typename Pred, typename Proj = ident>
                using Concept = concepts::models<ConceptImpl, R, Pred, Proj>;

                template<typename R, typename Pred, typename Proj = ident,
                    CONCEPT_REQUIRES_(Concept<R, Pred, Proj>())>
                R operator()(R && r, Pred pred, Proj proj = {}) const noexcept
                {
                    auto i = adjacent_remove_if(r, std::move(pred), std::move(proj));
                    erase(r, i, end(r));
                    return static_cast<R&&>(r);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename R, typename Pred, typename Proj = ident,
                    CONCEPT_REQUIRES_(!Concept<R, Pred, Proj>())>
                void operator()(R &&, Pred &&, Proj && = {}) const noexcept
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<R>(),
                        "The object on which action::adjacent_remove_if operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<R>;
                    CONCEPT_ASSERT_MSG(ErasableRange<R, I, I>(),
                        "The object on which action::adjacent_remove_if operates must allow element "
                        "removal.");
                    CONCEPT_ASSERT_MSG(IndirectInvocable<Proj, I>(),
                        "The projection function must accept objects of the iterator's value type, "
                        "reference type, and common reference type.");
                    CONCEPT_ASSERT_MSG(IndirectRelation<Pred, projected<I, Proj>>(),
                        "The predicate passed to action::adjacent_remove_if must accept objects returned "
                        "by the projection function, or of the range's value type if no projection "
                        "is specified.");
                    CONCEPT_ASSERT_MSG(Permutable<I>(),
                        "The iterator type of the range passed to action::adjacent_remove_if must allow its "
                        "elements to be permuted; that is, the values must be movable and the "
                        "iterator must be mutable.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \sa action
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(action<adjacent_remove_if_fn>, adjacent_remove_if)
        } // namespace action
        /// @}
    } // inline namespace v3
} // namespace ranges

#endif // RANGES_V3_ACTION_ADJACENT_REMOVE_IF_HPP
