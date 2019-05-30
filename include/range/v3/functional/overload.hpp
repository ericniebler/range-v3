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
#ifndef RANGES_V3_FUNCTIONAL_OVERLOAD_HPP
#define RANGES_V3_FUNCTIONAL_OVERLOAD_HPP

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    template<typename... Ts>
    struct overloaded;

    template<>
    struct overloaded<>
    {};

    template<typename First, typename... Rest>
    struct overloaded<First, Rest...>
    {
    private:
        RANGES_NO_UNIQUE_ADDRESS
        First first_;
        RANGES_NO_UNIQUE_ADDRESS
        overloaded<Rest...> second_;

    public:
        overloaded() = default;
        constexpr overloaded(First first, Rest... rest)
          : first_(static_cast<First &&>(first))
          , second_{static_cast<Rest &&>(rest)...}
        {}
        // clang-format off
        template<typename... Args>
        auto CPP_auto_fun(operator())(Args &&...args)
        (
            return invoke(first_, static_cast<Args &&>(args)...)
        )
        template<typename... Args>
        auto CPP_auto_fun(operator())(Args &&...args) (const)
        (
            return invoke((First const &) first_, static_cast<Args &&>(args)...)
        )
        template<typename... Args>
        auto CPP_auto_fun(operator())(Args &&...args)
        (
            return second_(static_cast<Args &&>(args)...)
        )
        template<typename... Args>
        auto CPP_auto_fun(operator())(Args &&...args) (const)
        (
            return ((overloaded<Rest...> const &) second_)(static_cast<Args &&>(args)...)
        )
        // clang-format on
    };

    struct overload_fn
    {
        template<typename Fn>
        constexpr Fn operator()(Fn fn) const
        {
            return fn;
        }
        template<typename... Fns>
        constexpr overloaded<Fns...> operator()(Fns... fns) const
        {
            return overloaded<Fns...>{static_cast<Fns &&>(fns)...};
        }
    };

    /// \ingroup group-functional
    /// \sa `overload_fn`
    RANGES_INLINE_VARIABLE(overload_fn, overload)
    /// @}
} // namespace ranges

#endif
