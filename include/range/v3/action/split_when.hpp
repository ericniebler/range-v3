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

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct split_when_fn
        {
        private:
            template<typename Rng>
            using split_value_t = meta::if_c<(bool)ranges::container<Rng>, uncvref_t<Rng>,
                                             std::vector<range_value_t<Rng>>>;

        public:
            // BUGBUG something is not right with the actions. It should be possible
            // to move a container into a split and have elements moved into the result.
            template<typename Rng, typename Fun>
            auto operator()(Rng && rng, Fun fun) const       //
                -> CPP_ret(std::vector<split_value_t<Rng>>)( //
                    requires forward_range<Rng> &&           //
                        invocable<Fun &, iterator_t<Rng>, sentinel_t<Rng>> && invocable<
                            Fun &, iterator_t<Rng>, iterator_t<Rng>> &&
                            copy_constructible<Fun> && convertible_to<
                                invoke_result_t<Fun &, iterator_t<Rng>, sentinel_t<Rng>>,
                                std::pair<bool, iterator_t<Rng>>>)
            {
                return views::split_when(rng, std::move(fun)) |
                       views::transform(to<split_value_t<Rng>>()) | to_vector;
            }
            template<typename Rng, typename Fun>
            auto operator()(Rng && rng, Fun fun) const
                -> CPP_ret(std::vector<split_value_t<Rng>>)( //
                    requires forward_range<Rng> && predicate<
                        Fun const &, range_reference_t<Rng>> && copy_constructible<Fun>)
            {
                return views::split_when(rng, std::move(fun)) |
                       views::transform(to<split_value_t<Rng>>()) | to_vector;
            }
        };

        /// \ingroup group-actions
        /// \relates split_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<split_when_fn>, split_when)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
