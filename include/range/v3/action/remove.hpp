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
#ifndef RANGES_V3_ACTION_REMOVE_HPP
#define RANGES_V3_ACTION_REMOVE_HPP

#include <meta/meta.hpp>
#include <range/v3/core.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/remove.hpp>
#include <range/v3/action/erase.hpp>


namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct remove_fn
            {
            private:
                struct ComparableWithRangeRef_
                {
                    template<typename Val, typename Rng,
                        typename Ref = range_reference_t<Rng>>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::EqualityComparable, Ref, Val>()
                        ));
                };

                friend action_access;
                template<typename V, typename P,
                    CONCEPT_REQUIRES_(!(Range<V>() &&
                        concepts::models<ComparableWithRangeRef_, P, V>()))>
                static auto bind(remove_fn remove, V &&value, P proj)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(remove,
                        std::placeholders::_1,
                        bind_forward<V>(value),
                        protect(std::move(proj))
                    )
                )

                template<typename V>
                static auto bind(remove_fn remove, V &&value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(remove,
                        std::placeholders::_1,
                        bind_forward<V>(value),
                        ident{}
                    )
                )
            public:
                struct ConceptImpl
                {
                    template<typename Rng, typename V, typename P = ident,
                        typename I = iterator_t<Rng>>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::ForwardRange, Rng>(),
                            concepts::model_of<concepts::ErasableRange, Rng, I, I>(),
                            concepts::is_true(Removable<I, V, P>())
                        ));
                };

                template<typename Rng, typename V, typename P = ident>
                using Concept = concepts::models<ConceptImpl, Rng, V, P>;

                template<typename Rng, typename V, typename P = ident,
                    CONCEPT_REQUIRES_(Concept<Rng, V, P>())>
                Rng operator()(Rng &&rng, V const &value, P proj = {}) const
                {
                    auto it = ranges::remove(rng, value, std::move(proj));
                    ranges::erase(rng, it, ranges::end(rng));
                    return static_cast<Rng&&>(rng);
                }
            };


            /// \ingroup group-actions
            /// \sa action
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(action<remove_fn>, remove)
        }
        /// @}
    }
}

#endif // include guard
