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
            template<typename Rng>
            using join_action_value_t_ =
                meta::if_c<
                    (bool) ranges::Container<range_value_t<Rng>>,
                    range_value_t<Rng>,
                    std::vector<range_value_t<range_value_t<Rng>>>>;

            CPP_def
            (
                template(typename Rng)
                concept JoinActionConcept,
                    InputRange<Rng> &&
                    InputRange<range_value_t<Rng>> &&
                    Semiregular<join_action_value_t_<Rng>>
            );

            struct join_fn
            {
            public:
                template<typename Rng>
                auto operator()(Rng &&rng) const -> CPP_ret(join_action_value_t_<Rng>)(
                    requires JoinActionConcept<Rng>)
                {
                    join_action_value_t_<Rng> ret;
                    auto end = ranges::end(rng);
                    for(auto it = begin(rng); it != end; ++it)
                        push_back(ret, *it);
                    return ret;
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng>
                auto operator()(Rng &&) const -> CPP_ret(void)(
                    requires not JoinActionConcept<Rng>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object on which action::join operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(InputRange<range_value_t<Rng>>,
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
