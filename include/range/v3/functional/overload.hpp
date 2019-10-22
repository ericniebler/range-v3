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

#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

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

#if RANGES_CXX_IF_CONSTEXPR < RANGES_CXX_IF_CONSTEXPR_17
        template<typename This, typename... Args>
        static constexpr decltype(auto) select(std::true_type, This && ovr,
                                               Args &&... args)
        {
            return invoke(((This &&) ovr).first_, (Args &&) args...);
        }
        template<typename This, typename... Args>
        static constexpr decltype(auto) select(std::false_type, This && ovr,
                                               Args &&... args)
        {
            return invoke(((This &&) ovr).second_, (Args &&) args...);
        }
#endif

    public:
        overloaded() = default;
        constexpr overloaded(First first, Rest... rest)
          : first_(static_cast<First &&>(first))
          , second_{static_cast<Rest &&>(rest)...}
        {}
        CPP_template(typename... Args)( //
            requires(defer::invocable<First &, Args...> ||
                     defer::invocable<overloaded<Rest...> &, Args...>)) //
            constexpr decltype(auto)
            operator()(Args &&... args) &
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr((bool)invocable<First &, Args...>)
                return invoke(first_, (Args &&) args...);
            else
                return invoke(second_, (Args &&) args...);
#else
            return overloaded::select(
                meta::bool_<invocable<First &, Args...>>{}, *this, (Args &&) args...);
#endif
        }
        CPP_template(typename... Args)( //
            requires(defer::invocable<First const &, Args...> ||
                     defer::invocable<overloaded<Rest...> const &, Args...>)) //
            constexpr decltype(auto)
            operator()(Args &&... args) const &
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr((bool)invocable<First const &, Args...>)
                return invoke(first_, (Args &&) args...);
            else
                return invoke(second_, (Args &&) args...);
#else
            return overloaded::select(meta::bool_<invocable<First const &, Args...>>{},
                                      *this,
                                      (Args &&) args...);
#endif
        }
        CPP_template(typename... Args)( //
            requires(defer::invocable<First, Args...> ||
                     defer::invocable<overloaded<Rest...>, Args...>)) //
            constexpr decltype(auto)
            operator()(Args &&... args) &&
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr((bool)invocable<First const &, Args...>)
                return invoke((First &&) first_, (Args &&) args...);
            else
                return invoke((overloaded<Rest...> &&) second_, (Args &&) args...);
#else
            return overloaded::select(meta::bool_<invocable<First, Args...>>{},
                                      (overloaded &&) * this,
                                      (Args &&) args...);
#endif
        }
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

#include <range/v3/detail/reenable_warnings.hpp>

#endif
