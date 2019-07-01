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

#ifndef RANGE_V3_FUNCTIONAL_BIND_FRONT_HPP
#define RANGE_V3_FUNCTIONAL_BIND_FRONT_HPP

#include <tuple>
#include <utility>

namespace ranges
{
    template<typename Fn, typename... Args>
    class bind_front_t
    {
        Fn fn;
        std::tuple<Args...> args_tuple;

        template<typename Rng, std::size_t... I>
        constexpr decltype(auto) apply(Rng && rng, std::index_sequence<I...>)
        {
            return fn(std::forward<Rng>(rng), std::get<I>(args_tuple)...);
        }

    public:
        constexpr bind_front_t(Fn fn, Args... args)
          : fn(std::move(fn))
          , args_tuple(std::move(args)...)
        {}

        template<typename Rng>
        constexpr decltype(auto) operator()(Rng && rng)
        {
            return apply(std::forward<Rng>(rng), std::index_sequence_for<Args...>{});
        }
    };

    template<typename Fn, typename... Args>
    constexpr auto bind_front(Fn fn, Args... args)
    {
        return bind_front_t<Fn, Args...>(fn, std::move(args)...);
    }
} // namespace ranges

#endif // RANGE_V3_FUNCTIONAL_BIND_FRONT_HPP
