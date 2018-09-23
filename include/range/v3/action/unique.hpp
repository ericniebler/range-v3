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

#ifndef RANGES_V3_ACTION_UNIQUE_HPP
#define RANGES_V3_ACTION_UNIQUE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            CPP_def
            (
                template(typename Rng, typename C = equal_to, typename P = ident)
                (concept UniqueActionConcept)(Rng, C, P),
                    ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                    Sortable<iterator_t<Rng>, C, P>
            );

            struct unique_fn
            {
            private:
                friend action_access;
                CPP_template(typename C, typename P = ident)(
                    requires not Range<C>)
                static auto bind(unique_fn unique, C pred, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(unique, std::placeholders::_1, protect(std::move(pred)),
                        protect(std::move(proj)))
                )
            public:
                CPP_template(typename Rng, typename C = equal_to, typename P = ident)(
                    requires UniqueActionConcept<Rng, C, P>)
                Rng operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
                {
                    auto it = ranges::unique(rng, std::move(pred), std::move(proj));
                    ranges::erase(rng, it, end(rng));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Rng, typename C = equal_to, typename P = ident)(
                    requires not UniqueActionConcept<Rng, C, P>)
                void operator()(Rng &&, C && = C{}, P && = P{}) const
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which action::unique operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    using S = sentinel_t<Rng>;
                    CPP_assert_msg(ErasableRange<Rng &, I, S>,
                        "The object on which action::unique operates must allow element "
                        "removal.");
                    CPP_assert_msg(IndirectInvocable<P, I>,
                        "The projection function must accept objects of the iterator's value type, "
                        "reference type, and common reference type.");
                    CPP_assert_msg(IndirectRelation<C, projected<I, P>>,
                        "The comparator passed to action::unique must accept objects returned "
                        "by the projection function, or of the range's value type if no projection "
                        "is specified.");
                    CPP_assert_msg(Permutable<I>,
                        "The iterator type of the range passed to action::unique must allow its "
                        "elements to be permuted; that is, the values must be movable and the "
                        "iterator must be mutable.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates unique_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<unique_fn>, unique)
        }
        /// @}
    }
}

#endif
