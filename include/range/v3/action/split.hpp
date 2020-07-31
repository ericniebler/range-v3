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

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/split.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct split_fn
        {
            template<typename Rng>
            using split_value_t =
                meta::if_c<(bool)ranges::container<Rng>, //
                           uncvref_t<Rng>, std::vector<range_value_t<Rng>>>;

            template<typename T>
            constexpr auto CPP_fun(operator())(T & t)(const //
                requires range<T &>)
            {
                return make_action_closure(
                    bind_back(split_fn{}, detail::reference_wrapper_<T>(t)));
            }

            template<typename T>
            constexpr auto operator()(T && t) const
            {
                return make_action_closure(bind_back(split_fn{}, static_cast<T &&>(t)));
            }

            // BUGBUG something is not right with the actions. It should be possible
            // to move a container into a split and have elements moved into the result.
            template(typename Rng)( //
                requires input_range<Rng> AND indirectly_comparable<
                        iterator_t<Rng>, range_value_t<Rng> const *, ranges::equal_to>) //
            auto operator()(Rng && rng, range_value_t<Rng> val) const
                -> std::vector<split_value_t<Rng>>
            {
                return views::split(rng, std::move(val)) |
                       to<std::vector<split_value_t<Rng>>>();
            }

            template(typename Rng, typename Pattern)( //
                requires input_range<Rng> AND viewable_range<Pattern> AND
                        forward_range<Pattern> AND indirectly_comparable<
                            iterator_t<Rng>, iterator_t<Pattern>, ranges::equal_to> &&
                    (forward_range<Rng> || detail::tiny_range<Pattern>)) //
            auto operator()(Rng && rng, Pattern && pattern) const
                -> std::vector<split_value_t<Rng>>
            {
                return views::split(rng, static_cast<Pattern &&>(pattern)) |
                       to<std::vector<split_value_t<Rng>>>();
            }

            /// \cond
            template<typename Rng, typename T>
            auto operator()(Rng && rng, detail::reference_wrapper_<T> r) const
                -> invoke_result_t<split_fn, Rng, T &>
            {
                return (*this)(static_cast<Rng &&>(rng), r.get());
            }
            /// \endcond
        };

        /// \relates actions::split_fn
        RANGES_INLINE_VARIABLE(split_fn, split)
    } // namespace actions
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
