/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_REMOVE_HPP
#define RANGES_V3_VIEW_REMOVE_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/view.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct remove_base_fn
        {
        private:
            template<typename Value>
            struct pred_
            {
                Value value_;
                template<typename T>
                auto operator()(T && other) const -> CPP_ret(bool)( //
                    requires equality_comparable_with<T, Value const &>)
                {
                    return static_cast<T &&>(other) == value_;
                }
            };

        public:
            template<typename Rng, typename Value>
            constexpr auto CPP_fun(operator())(Rng && rng, Value value)(
                const requires move_constructible<Value> && viewable_range<Rng> &&
                    input_range<Rng> &&
                        indirectly_comparable<iterator_t<Rng>, Value const *, equal_to>)
            {
                return remove_if(static_cast<Rng &&>(rng),
                                 pred_<Value>{std::move(value)});
            }

            template<typename Rng, typename Value, typename Proj>
            constexpr auto CPP_fun(operator())(Rng && rng, Value value, Proj proj)(
                const requires move_constructible<Value> && viewable_range<Rng> &&
                    input_range<Rng> && indirectly_comparable<
                        iterator_t<Rng>, Value const *, equal_to, Proj>)
            {
                return remove_if(static_cast<Rng &&>(rng),
                                 pred_<Value>{std::move(value)},
                                 std::move(proj));
            }
        };

        struct remove_bind_fn
        {
            template<typename Value>
            constexpr auto operator()(Value value) const // TODO: underconstrained
            {
                return make_view_closure(bind_back(remove_base_fn{}, std::move(value)));
            }
            template<typename Value, typename Proj>
            constexpr auto CPP_fun(operator())(Value && value,
                                               Proj proj)(const //
                                                          requires(!range<Value>)) // TODO: underconstrained
            {
                return make_view_closure(bind_back(
                    remove_base_fn{}, static_cast<Value &&>(value), std::move(proj)));
            }
        };

        struct RANGES_EMPTY_BASES remove_fn
          : remove_base_fn, remove_bind_fn
        {
            using remove_base_fn::operator();
            using remove_bind_fn::operator();
        };

        /// \relates remove_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(remove_fn, remove)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif // RANGES_V3_VIEW_REMOVE_HPP
