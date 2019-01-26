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
#ifndef RANGES_V3_FUNCTIONAL_PIPEABLE_HPP
#define RANGES_V3_FUNCTIONAL_PIPEABLE_HPP

#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Bind>
        struct RANGES_EMPTY_BASES pipeable_binder
          : Bind
          , pipeable<pipeable_binder<Bind>>
        {
            pipeable_binder(Bind bind)
              : Bind(static_cast<Bind &&>(bind))
            {}
        };

        template<typename Pipe0, typename Pipe1>
        struct composed_pipe
        {
            Pipe0 pipe0_;
            Pipe1 pipe1_;
            template<typename Arg>
            auto CPP_auto_fun(operator())(Arg &&arg) (const)
            (
                return static_cast<Arg &&>(arg) | pipe0_ | pipe1_
            )
        };
    }
    /// \endcond

    struct make_pipeable_fn
    {
        template<typename Fun>
        detail::pipeable_binder<Fun> operator()(Fun fun) const
        {
            return {static_cast<Fun &&>(fun)};
        }
    };

    /// \ingroup group-utility
    /// \sa `make_pipeable_fn`
    RANGES_INLINE_VARIABLE(make_pipeable_fn, make_pipeable)

    struct pipeable_base;

    template<typename T>
    /*inline*/ constexpr bool is_pipeable_v = std::is_base_of<pipeable_base, T>::value;

    template<typename T>
    /*inline*/ constexpr bool is_pipeable_v<T &> = std::is_base_of<pipeable_base, T>::value;

    template<typename T>
    /*inline*/ constexpr bool is_pipeable_v<T &&> = std::is_base_of<pipeable_base, T>::value;

    template<typename T>
    using is_pipeable = meta::bool_<is_pipeable_v<T>>;

    struct pipeable_access
    {
        template<typename Pipeable>
        struct impl_
          : Pipeable
        {
            using Pipeable::pipe;
        };

        template<typename Pipeable>
        struct impl_<Pipeable &>
          : impl_<Pipeable>
        {};

        template<typename T>
        using impl = meta::if_c<is_pipeable_v<T>, impl_<T>>;
    };

    struct pipeable_base
    {
        // Evaluate the pipe with an argument
        template<typename Arg, typename Pipe>
        friend auto operator|(Arg &&arg, Pipe pipe) ->
            CPP_ret(decltype(pipeable_access::impl<Pipe>::pipe(
                std::declval<Arg>(), std::declval<Pipe &>())))(
            requires (!is_pipeable_v<Arg>) && is_pipeable_v<Pipe>)
        {
            return pipeable_access::impl<Pipe>::pipe(static_cast<Arg &&>(arg), pipe);
        }

        // Compose two pipes
        template<typename Pipe0, typename Pipe1>
        friend auto operator|(Pipe0 pipe0, Pipe1 pipe1) ->
            CPP_ret(decltype(make_pipeable(
                std::declval<detail::composed_pipe<Pipe0, Pipe1>>())))(
            requires is_pipeable_v<Pipe0> && is_pipeable_v<Pipe1>)
        {
            return make_pipeable(detail::composed_pipe<Pipe0, Pipe1>{pipe0, pipe1});
        }
    };

    template<typename Derived>
    struct pipeable
      : pipeable_base
    {
    private:
        friend pipeable_access;
        // Default Pipe behavior just passes the argument to the pipe's function call
        // operator
        template<typename Arg, typename Pipe>
        static auto CPP_auto_fun(pipe)(Arg &&arg, Pipe p)
        (
            return p(static_cast<Arg &&>(arg))
        )
    };
}

#endif
