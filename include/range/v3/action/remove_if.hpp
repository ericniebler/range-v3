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

#ifndef RANGES_V3_ACTION_REMOVE_IF_HPP
#define RANGES_V3_ACTION_REMOVE_IF_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        // TODO Look at all the special cases handled by erase_if in Library Fundamentals 2

        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            CPP_def
            (
                template(typename Rng, typename C, typename P = ident)
                (concept RemoveIfActionConcept)(Rng, C, P),
                    ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, iterator_t<Rng>> &&
                    RemovableIf<iterator_t<Rng>, C, P>
            );

            struct remove_if_fn
            {
            private:
                friend action_access;
                template<typename C, typename P = ident>
                static auto CPP_fun(bind)(remove_if_fn remove_if, C pred, P proj = P{})(
                    requires not Range<C>)
                {
                    return std::bind(remove_if, std::placeholders::_1, protect(std::move(pred)),
                        protect(std::move(proj)));
                }
            public:
                CPP_template(typename Rng, typename C, typename P = ident)(
                    requires RemoveIfActionConcept<Rng, C, P>)
                Rng operator()(Rng &&rng, C pred, P proj = P{}) const
                {
                    auto it = ranges::remove_if(rng, std::move(pred), std::move(proj));
                    ranges::erase(rng, it, ranges::end(rng));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Rng, typename C, typename P = ident)(
                    requires not RemoveIfActionConcept<Rng, C, P>)
                void operator()(Rng &&, C &&, P && = P{}) const
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which action::remove_if operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    CPP_assert_msg(ErasableRange<Rng &, I, I>,
                        "The object on which action::remove_if operates must allow element "
                        "removal.");
                    CPP_assert_msg(IndirectInvocable<P, I>,
                        "The projection function must accept objects of the iterator's value type, "
                        "reference type, and common reference type.");
                    CPP_assert_msg(IndirectPredicate<C, projected<I, P>>,
                        "The predicate passed to action::remove_if must accept objects returned "
                        "by the projection function, or of the range's value type if no projection "
                        "is specified.");
                    CPP_assert_msg(Permutable<I>,
                        "The iterator type of the range passed to action::remove_if must allow its "
                        "elements to be permutaed; that is, the values must be movable and the "
                        "iterator must be mutable.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \sa action
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(action<remove_if_fn>, remove_if)
        }
        /// @}
    }
}

#endif
