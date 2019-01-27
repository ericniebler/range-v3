/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP
#define RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/core.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/unstable_remove_if.hpp>


namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct unstable_remove_if_fn
            {
            private:
                friend action_access;
                template<typename C, typename P = ident, CONCEPT_REQUIRES_(!Range<C>())>
                static auto bind(unstable_remove_if_fn unstable_remove_if, C pred, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(unstable_remove_if, std::placeholders::_1,
                              protect(std::move(pred)), protect(std::move(proj)))
                )
            public:

                struct ConceptImpl
                {
                    template<typename Rng, typename C, typename P = ident,
                            typename I = iterator_t<Rng>, typename S = sentinel_t<Rng>>
                    auto requires_() -> decltype(
                    concepts::valid_expr(
                            concepts::model_of<concepts::ErasableRange, Rng, I, S>(),
                            concepts::is_true(UnstableRemovableIf<I, S, C, P>())
                    ));
                };

                template<typename Rng, typename C, typename P = ident>
                using Concept = concepts::models<ConceptImpl, Rng, C, P>;

                template<typename Rng, typename C, typename P = ident,
                        CONCEPT_REQUIRES_(Concept<Rng, C, P>())>
                Rng operator()(Rng &&rng, C pred, P proj = P{}) const
                {
                    auto it = ranges::unstable_remove_if(ranges::begin(rng), ranges::end(rng), std::move(pred), std::move(proj));
                    ranges::erase(rng, it, ranges::end(rng));
                    return static_cast<Rng&&>(rng);
                }
            };


            /// \ingroup group-actions
            /// \sa action
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(action<unstable_remove_if_fn>, unstable_remove_if)
        }
        /// @}
    }
}

#endif // include guard
