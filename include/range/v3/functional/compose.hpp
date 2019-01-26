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
#ifndef RANGES_V3_FUNCTIONAL_COMPOSE_HPP
#define RANGES_V3_FUNCTIONAL_COMPOSE_HPP

#include <utility>
#include <type_traits>
#include <concepts/concepts.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    template<typename Second, typename First>
    struct composed
    {
    private:
        RANGES_NO_UNIQUE_ADDRESS
        First first_;
        RANGES_NO_UNIQUE_ADDRESS
        Second second_;

        template<typename A, typename B, typename...Ts>
        static auto CPP_auto_fun(do_)(A &a, B &b, std::false_type, Ts &&...ts)
        (
            return invoke(b, invoke(a, (Ts &&) ts...))
        )
        template<typename A, typename B, typename...Ts>
        static auto CPP_auto_fun(do_)(A &a, B &b, std::true_type, Ts &&...ts)
        (
            return (invoke(a, (Ts &&) ts...),
                invoke(b))
        )
    public:
        composed() = default;
        composed(Second second, First first)
          : first_(std::move(first))
          , second_(std::move(second))
        {}
        template<typename...Ts>
        auto CPP_auto_fun(operator())(Ts &&...ts)
        (
            return composed::do_(
                first_,
                second_,
                std::is_void<invoke_result_t<First &, Ts...>>{},
                (Ts &&) ts...)
        )
        template<typename...Ts>
        auto CPP_auto_fun(operator())(Ts &&...ts) (const)
        (
            return composed::do_(
                (First const &) first_,
                (Second const &) second_,
                std::is_void<invoke_result_t<First const &, Ts...>>{},
                (Ts &&) ts...)
        )
    };

    struct compose_fn
    {
        template<typename Second, typename First>
        composed<Second, First> operator()(Second second, First first) const
        {
            return {std::move(second), std::move(first)};
        }
    };

    /// \ingroup group-utility
    /// \sa `compose_fn`
    RANGES_INLINE_VARIABLE(compose_fn, compose)
}

#endif
