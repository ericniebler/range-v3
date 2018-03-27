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

#ifndef RANGES_V3_ACTION_JOIN_HPP
#define RANGES_V3_ACTION_JOIN_HPP

#include <vector>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/action/push_back.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct join_fn
            {
            private:
                template<typename Rng>
                using join_value_t =
                    meta::if_c<
                        (bool) ranges::Container<range_value_type_t<Rng>>(),
                        range_value_type_t<Rng>,
                        std::vector<range_value_type_t<range_value_type_t<Rng>>>>;

                struct Joinable_
                {
                    template<typename Rng>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::InputRange, Rng>(),
                            concepts::model_of<concepts::InputRange, range_value_type_t<Rng>>(),
                            concepts::model_of<concepts::SemiRegular, join_value_t<Rng>>()
                        ));
                };

            public:
                template<typename Rng>
                using Joinable = concepts::models<Joinable_, Rng>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Joinable<Rng>())>
                join_value_t<Rng> operator()(Rng && rng) const
                {
                    join_value_t<Rng> ret;
                    auto end = ranges::end(rng);
                    for(auto it = begin(rng); it != end; ++it)
                        push_back(ret, *it);
                    return ret;
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Joinable<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which action::join operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<range_value_type_t<Rng>>(),
                        "The Range on which action::join operates must have a value type that "
                        "models the InputRange concept.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates join_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<join_fn>, join)
        }
        /// @}
    }
}

#endif
