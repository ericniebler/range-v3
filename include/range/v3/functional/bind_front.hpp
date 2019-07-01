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
#include <type_traits>
#include <utility>

#include <range/v3/range_fwd.hpp>

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
        constexpr bind_front_t(Fn && fn, Args &&... args)
          : fn(std::move(fn))
          , args_tuple(std::move(args)...)
        {}

        template<typename Rng>
        constexpr decltype(auto) operator()(Rng && rng)
        {
            return apply(std::forward<Rng>(rng), std::index_sequence_for<Args...>{});
        }
    };

    // T/T&& => T&&, T& => T
    template<typename T,
             typename U = meta::if_<             //
                 std::is_lvalue_reference<T>,    //
                 uncvref_t<T>,                   //
                 std::remove_reference_t<T> &&>> //
    constexpr U bind_front_forward(std::remove_reference_t<T> & t) noexcept
    {
        return static_cast<U>(t);
    }

    template<typename Fn, typename... Args>
    constexpr auto bind_front(Fn && fn, Args &&... args)
    {
        return bind_front_t<std::decay_t<Fn>, std::decay_t<Args>...>(
            bind_front_forward<Fn>(fn), bind_front_forward<Args>(args)...);
    }

} // namespace ranges

#endif // RANGE_V3_FUNCTIONAL_BIND_FRONT_HPP
