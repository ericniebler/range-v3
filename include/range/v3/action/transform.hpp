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

#ifndef RANGES_V3_ACTION_TRANSFORM_HPP
#define RANGES_V3_ACTION_TRANSFORM_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct transform_fn
            {
            private:
                friend action_access;
                template<typename F, typename P = ident, CONCEPT_REQUIRES_(!Range<F>())>
                static auto bind(transform_fn transform, F fun, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(transform, std::placeholders::_1, protect(std::move(fun)),
                        protect(std::move(proj)))
                )
            public:
                struct ConceptImpl
                {
                    template<typename Rng, typename F, typename P = ident,
                        typename I = iterator_t<Rng>>
                        auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::InputRange, Rng>(),
                            concepts::is_true(Transformable1<I, I, F, P>())
                        ));
                };

                template<typename Rng, typename F, typename P = ident>
                using Concept = concepts::models<ConceptImpl, Rng, F, P>;

                template<typename Rng, typename F, typename P = ident,
                    CONCEPT_REQUIRES_(Concept<Rng, F, P>())>
                Rng operator()(Rng && rng, F fun, P proj = P{}) const
                {
                    ranges::transform(rng, begin(rng), std::move(fun), std::move(proj));
                    return static_cast<Rng&&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename F, typename P = ident,
                    CONCEPT_REQUIRES_(!Concept<Rng, F, P>())>
                void operator()(Rng &&, F &&, P && = P{}) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which action::transform operates must be a model of the "
                        "InputRange concept.");
                    using I = iterator_t<Rng>;
                    CONCEPT_ASSERT_MSG(IndirectInvocable<P, I>(),
                        "The projection function must accept objects of the iterator's value type, "
                        "reference type, and common reference type.");
                    CONCEPT_ASSERT_MSG(IndirectInvocable<F, projected<I, P>>(),
                        "The function argument to action::transform must be callable with "
                        "the result of the projection argument, or with objects of the range's "
                        "common reference type if no projection is specified.");
                    CONCEPT_ASSERT_MSG(Writable<iterator_t<Rng>,
                            concepts::Invocable::result_t<F&,
                                concepts::Invocable::result_t<P&, range_common_reference_t<Rng>>>>(),
                        "The result type of the function passed to action::transform must "
                        "be writable back into the source range.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates transform_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<transform_fn>, transform)
        }
        /// @}
    }
}

#endif
