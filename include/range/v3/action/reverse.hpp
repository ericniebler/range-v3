/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_REVERSE_HPP
#define RANGES_V3_ACTION_REVERSE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/reverse.hpp>
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
            CONCEPT_def
            (
                template(typename Rng)
                concept ReverseActionConcept,
                    BidirectionalRange<Rng> &&
                    Permutable<iterator_t<Rng>>
            );

            /// Reversed the source range in-place.
            struct reverse_fn
            {
            private:
                friend action_access;

            public:
                CONCEPT_template(typename Rng)(
                    requires ReverseActionConcept<Rng>)
                Rng operator()(Rng &&rng) const
                {
                    ranges::reverse(rng);
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CONCEPT_template(typename Rng)(
                    requires not ReverseActionConcept<Rng>)
                void operator()(Rng &&) const
                {
                    CONCEPT_assert_msg(BidirectionalRange<Rng>,
                        "The object on which action::reverse operates must be a model of the "
                        "BidirectionalRange concept.");
                    CONCEPT_assert_msg(Permutable<iterator_t<Rng>>,
                        "The iterator type of the range passed to action::reverse must allow its "
                        "elements to be permuted; that is, the values must be movable and the "
                        "iterator must be mutable.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates reverse_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<reverse_fn>, reverse)
        }
        /// @}
    }
}

#endif
