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

#ifndef RANGE_V3_DETAIL_BIND_BACK_HPP
#define RANGE_V3_DETAIL_BIND_BACK_HPP

#include <tuple>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>

namespace ranges
{
    // bind_back like std::bind_front has no special treatment for nested
    // bind-expressions or reference_wrappers; there is no need to wrap
    // Callables with ranges::protect.
    namespace detail
    {
        template<typename Fn, typename... Args>
        struct bind_back_
        {
            Fn fn_;
            std::tuple<Args...> args_tuple_;

            template<typename... CallArgs>
                constexpr auto operator()(CallArgs &&... cargs) &&
                noexcept(is_nothrow_invocable_v<Fn, CallArgs..., Args...>)
                    -> invoke_result_t<Fn, CallArgs..., Args...>
            {
                return tuple_apply(
                    [&, this](auto &&... args) -> decltype(auto) {
                        return invoke(static_cast<Fn &&>(fn_),
                                      static_cast<CallArgs &&>(cargs)...,
                                      static_cast<decltype(args)>(args)...);
                    },
                    static_cast<std::tuple<Args...> &&>(args_tuple_));
            }

            /// \overload
            template<typename... CallArgs>
                constexpr auto operator()(CallArgs &&... cargs) &
                noexcept(is_nothrow_invocable_v<Fn &, CallArgs..., Args &...>)
                    -> invoke_result_t<Fn &, CallArgs..., Args &...>
            {
                return tuple_apply(
                    [&, this](auto &&... args) -> decltype(auto) {
                        return invoke(fn_,
                                      static_cast<CallArgs &&>(cargs)...,
                                      static_cast<decltype(args)>(args)...);
                    },
                    args_tuple_);
            }

            /// \overload
            template<typename... CallArgs>
            constexpr auto operator()(CallArgs &&... cargs) const
                noexcept(is_nothrow_invocable_v<Fn const &, CallArgs..., Args const &...>)
                    -> invoke_result_t<Fn const &, CallArgs..., Args const &...>
            {
                return tuple_apply(
                    [&, this](auto &&... args) -> decltype(auto) {
                        return invoke(fn_,
                                      static_cast<CallArgs &&>(cargs)...,
                                      static_cast<decltype(args)>(args)...);
                    },
                    args_tuple_);
            }
        };
    } // namespace detail

    struct bind_back_fn
    {
        template<typename Fn, typename... Args>
        constexpr auto operator()(Fn && fn, Args &&... args) const
        {
            return detail::bind_back_<detail::decay_t<Fn>, detail::decay_t<Args>...>{
                static_cast<Fn &&>(fn), {static_cast<Args &&>(args)...}};
        }
    };

    /// \ingroup group-utility
    /// \sa `bind_back_fn`
    RANGES_INLINE_VARIABLE(bind_back_fn, bind_back)

} // namespace ranges

#endif // RANGE_V3_DETAIL_BIND_BACK_HPP
