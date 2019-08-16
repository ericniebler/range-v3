
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
#ifndef RANGES_V3_FUNCTIONAL_CONCEPTS_HPP
#define RANGES_V3_FUNCTIONAL_CONCEPTS_HPP

#include <concepts/concepts.hpp>

#include <range/v3/functional/invoke.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{

    // clang-format off
    CPP_def
    (
        template(typename Fun, typename... Args)
        (concept invocable)(Fun, Args...),
            requires (Fun&& fn)
            (
                invoke(static_cast<Fun &&>(fn), std::declval<Args>()...)
            )
    );

    CPP_def
    (
        template(typename Fun, typename... Args)
        (concept regular_invocable)(Fun, Args...),
            invocable<Fun, Args...>
            // Axiom: equality_preserving(invoke(f, args...))
    );

    CPP_def
    (
        template(typename Fun, typename... Args)
        (concept predicate)(Fun, Args...),
            regular_invocable<Fun, Args...> &&
            convertible_to<invoke_result_t<Fun, Args...>, bool>
    );

    CPP_def
    (
        template(typename R, typename T, typename U)
        concept relation,
            predicate<R, T, T> &&
            predicate<R, U, U> &&
            predicate<R, T, U> &&
            predicate<R, U, T>
    );

    CPP_def
    (
        template(typename R, typename T, typename U)
        concept strict_weak_order,
            relation<R, T, U>
    );
    // clang-format on

    namespace cpp20
    {
        using ranges::invocable;
        using ranges::predicate;
        using ranges::regular_invocable;
        using ranges::relation;
        using ranges::strict_weak_order;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif
