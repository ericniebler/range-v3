
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
    /// \addtogroup group-concepts
    /// @{
    CPP_def
    (
        template(typename Fun, typename... Args)
        (concept Invocable)(Fun, Args...),
            requires (Fun&& fn)
            (
                invoke(static_cast<Fun &&>(fn), std::declval<Args>()...)
            )
    );

    CPP_def
    (
        template(typename Fun, typename... Args)
        (concept RegularInvocable)(Fun, Args...),
            Invocable<Fun, Args...>
            // Axiom: equality_preserving(invoke(f, args...))
    );

    CPP_def
    (
        template(typename Fun, typename... Args)
        (concept Predicate)(Fun, Args...),
            RegularInvocable<Fun, Args...> &&
            ConvertibleTo<invoke_result_t<Fun, Args...>, bool>
    );

    CPP_def
    (
        template(typename R, typename T, typename U)
        concept Relation,
            Predicate<R, T, T> &&
            Predicate<R, U, U> &&
            Predicate<R, T, U> &&
            Predicate<R, U, T>
    );

    CPP_def
    (
        template(typename R, typename T, typename U)
        concept StrictWeakOrder,
            Relation<R, T, U>
    );
}

#endif
