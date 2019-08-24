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
    /// \addtogroup group-functional
    /// @{

    struct pipeable_base;

    template<typename T>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v = META_IS_BASE_OF(pipeable_base, T);

    template<typename T>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<T &> = META_IS_BASE_OF(pipeable_base,
                                                                          T);
    template<typename T>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<T &&> = META_IS_BASE_OF(pipeable_base,
                                                                           T);
    template<typename T>
    using is_pipeable = meta::bool_<is_pipeable_v<T>>;

    struct pipeable_access
    {
        template<typename Pipeable>
        struct impl : Pipeable
        {
            using Pipeable::pipe;
        };
    };

    struct pipeable_base
    {
    private:
        friend pipeable_access;

        template<typename Pipe0, typename Pipe1>
        struct composed;

        // Evaluate the pipe with an argument
        CPP_template(typename Arg, typename Pipe)( //
            requires(!is_pipeable_v<Arg>) && is_pipeable_v<Pipe>) // clang-format off
        constexpr friend auto operator|(Arg &&arg, Pipe pipe) // clang-format off
        {
            return pipeable_access::impl<Pipe>::pipe(static_cast<Arg &&>(arg), pipe);
        }

        // Compose two pipes
        CPP_template(typename Pipe0, typename Pipe1)( //
            requires is_pipeable_v<Pipe0> && is_pipeable_v<Pipe1>) // clang-format off
        constexpr friend auto operator|(Pipe0 pipe0, Pipe1 pipe1)    // clang-format on
        {
            return composed<Pipe0, Pipe1>{pipe0, pipe1};
        }

        // Default Pipe behavior just passes the argument to the pipe's function call
        // operator
        // clang-format off
        template<typename Arg, typename Pipe>
        constexpr static auto CPP_auto_fun(pipe)(Arg &&arg, Pipe p)
        (
            return static_cast<Pipe &&>(p)(static_cast<Arg &&>(arg))
        )
        // clang-format on
    };

    template<typename Pipe0, typename Pipe1>
    struct pipeable_base::composed : pipeable_base
    {
        Pipe0 pipe0_;
        Pipe1 pipe1_;
        constexpr composed(Pipe0 pipe0, Pipe1 pipe1)
          : pipe0_(static_cast<Pipe0 &&>(pipe0))
          , pipe1_(static_cast<Pipe1 &&>(pipe1))
        {}
        // clang-format off
        template<typename Arg>
        constexpr auto CPP_auto_fun(operator())(Arg &&arg) (const)
        (
            return static_cast<Arg &&>(arg) | pipe0_ | pipe1_
        )
        // clang-format on
    };

    struct make_pipeable_fn
    {
        template<typename Fun>
        constexpr auto operator()(Fun fun) const
        {
            struct _
              : Fun
              , pipeable_base
            {
                constexpr explicit _(Fun && f)
                  : Fun(static_cast<Fun &&>(f))
                {}
            };
            return _{static_cast<Fun &&>(fun)};
        }
    };

    /// \ingroup group-functional
    /// \sa `make_pipeable_fn`
    RANGES_INLINE_VARIABLE(make_pipeable_fn, make_pipeable)

    template<typename>
    using pipeable RANGES_DEPRECATED("Please use pipeable_base instead") = pipeable_base;

    /// \endcond

    /// @}
} // namespace ranges

#endif
