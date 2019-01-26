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

#ifndef RANGES_V3_ACTION_SPLIT_WHEN_HPP
#define RANGES_V3_ACTION_SPLIT_WHEN_HPP

#include <vector>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace action
    {
        struct split_when_fn
        {
        private:
            template<typename Rng>
            using split_value_t =
                meta::if_c<
                    (bool) ranges::Container<Rng>,
                    uncvref_t<Rng>,
                    std::vector<range_value_t<Rng>>>;
        public:
            // BUGBUG something is not right with the actions. It should be possible
            // to move a container into a split and have elements moved into the result.
            CPP_template(typename Rng, typename Fun)(
                requires ForwardRange<Rng> &&
                    Invocable<Fun&, iterator_t<Rng>, sentinel_t<Rng>> &&
                    Invocable<Fun&, iterator_t<Rng>, iterator_t<Rng>> &&
                    CopyConstructible<Fun> &&
                    ConvertibleTo<
                        invoke_result_t<Fun&, iterator_t<Rng>, sentinel_t<Rng>>,
                        std::pair<bool, iterator_t<Rng>>>)
            std::vector<split_value_t<Rng>> operator()(Rng &&rng, Fun fun) const
            {
                return view::split_when(rng, std::move(fun))
                     | view::transform(to<split_value_t<Rng>>()) | to_vector;
            }
            CPP_template(typename Rng, typename Fun)(
                requires ForwardRange<Rng> &&
                    Predicate<Fun const&, range_reference_t<Rng>> &&
                    CopyConstructible<Fun>)
            std::vector<split_value_t<Rng>> operator()(Rng &&rng, Fun fun) const
            {
                return view::split_when(rng, std::move(fun))
                     | view::transform(to<split_value_t<Rng>>()) | to_vector;
            }
        };

        /// \ingroup group-actions
        /// \relates split_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<split_when_fn>, split_when)
    }
    /// @}
}

#endif
