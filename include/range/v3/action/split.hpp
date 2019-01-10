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

#ifndef RANGES_V3_ACTION_SPLIT_HPP
#define RANGES_V3_ACTION_SPLIT_HPP

#include <vector>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace action
    {
        struct split_fn
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
                requires view::SplitOnFunction<Rng, Fun>)
            std::vector<split_value_t<Rng>> operator()(Rng &&rng, Fun fun) const
            {
                return view::split(rng, std::move(fun))
                     | view::transform(to_<split_value_t<Rng>>()) | to_vector;
            }
            CPP_template(typename Rng, typename Fun)(
                requires view::SplitOnPredicate<Rng, Fun>)
            std::vector<split_value_t<Rng>> operator()(Rng &&rng, Fun fun) const
            {
                return view::split(rng, std::move(fun))
                     | view::transform(to_<split_value_t<Rng>>()) | to_vector;
            }
            CPP_template(typename Rng)(
                requires view::SplitOnElement<Rng>)
            std::vector<split_value_t<Rng>> operator()(Rng &&rng, range_value_t<Rng> val) const
            {
                return view::split(rng, std::move(val))
                     | view::transform(to_<split_value_t<Rng>>()) | to_vector;
            }
            CPP_template(typename Rng, typename Sub)(
                requires view::SplitOnSubRange<Rng, Sub>)
            std::vector<split_value_t<Rng>> operator()(Rng &&rng, Sub &&sub) const
            {
                return view::split(rng, static_cast<Sub &&>(sub))
                     | view::transform(to_<split_value_t<Rng>>()) | to_vector;
            }
        };

        /// \ingroup group-actions
        /// \relates split_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<split_fn>, split)
    }
    /// @}
}

#endif
