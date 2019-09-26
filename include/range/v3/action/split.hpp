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

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct split_fn
        {
        private:
            friend action_access;
            template<typename Rng>
            using split_value_t = meta::if_c<(bool)ranges::container<Rng>, uncvref_t<Rng>,
                                             std::vector<range_value_t<Rng>>>;

            template<typename T>
            static auto bind(split_fn split, T && val)
            {
                return bind_back(split, static_cast<T &&>(val));
            }
#ifdef RANGES_WORKAROUND_MSVC_OLD_LAMBDA
            template<typename T, std::size_t N>
            struct lamduh
            {
                T (&val_)[N];

                template<class Rng>
                constexpr auto operator()(Rng && rng)
                    -> decltype(split_fn{}(std::declval<Rng>(), val_))
                {
                    return split_fn{}(static_cast<Rng &&>(rng), val_);
                }
            };

            template<typename T, std::size_t N>
            static auto bind(split_fn, T (&val)[N])
            {
                return lamduh<T, N>{val};
            }
#else  // ^^^ workaround / no workaround vvv
            template<typename T, std::size_t N>
            static auto bind(split_fn, T (&val)[N])
            {
                return [&val](auto && rng)
                           -> invoke_result_t<split_fn, decltype(rng), T(&)[N]>
                {
                    return split_fn{}(static_cast<decltype(rng)>(rng), val);
                };
            }
#endif // RANGES_WORKAROUND_MSVC_OLD_LAMBDA

        public:
            // BUGBUG something is not right with the actions. It should be possible
            // to move a container into a split and have elements moved into the result.
            template<typename Rng>
            auto operator()(Rng && rng, range_value_t<Rng> val) const
                -> CPP_ret(std::vector<split_value_t<Rng>>)( //
                    requires input_range<Rng> && indirectly_comparable<
                        iterator_t<Rng>, range_value_t<Rng> const *, ranges::equal_to>)
            {
                return views::split(rng, std::move(val)) |
                       to<std::vector<split_value_t<Rng>>>();
            }
            template<typename Rng, typename Pattern>
            auto operator()(Rng && rng, Pattern && pattern) const
                -> CPP_ret(std::vector<split_value_t<Rng>>)( //
                    requires input_range<Rng> && viewable_range<Pattern> &&
                        forward_range<Pattern> && indirectly_comparable<
                            iterator_t<Rng>, iterator_t<Pattern>, ranges::equal_to> &&
                    (forward_range<Rng> || detail::tiny_range<Pattern>))
            {
                return views::split(rng, static_cast<Pattern &&>(pattern)) |
                       to<std::vector<split_value_t<Rng>>>();
            }
        };

        /// \ingroup group-actions
        /// \relates split_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<split_fn>, split)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
