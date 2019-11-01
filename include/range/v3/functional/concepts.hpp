
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

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{

    // clang-format off
    template<typename Fun, typename... Args>
    CPP_concept_bool invocable =
        CPP_requires ((Fun&&) fn) //
        (
            invoke(CPP_fwd(fn), std::declval<Args>()...)
        );

    template<typename Fun, typename... Args>
    CPP_concept_bool regular_invocable =
        invocable<Fun, Args...>;
        // Axiom: equality_preserving(invoke(f, args...))

    template<typename Fun, typename... Args>
    CPP_concept_bool predicate =
        regular_invocable<Fun, Args...> &&
        CPP_requires ((Fun&&) fn) //
        (
            concepts::requires_<
                convertible_to<
                    decltype(invoke(CPP_fwd(fn), std::declval<Args>()...)),
                    bool>>
        );

    template<typename R, typename T, typename U>
    CPP_concept_bool relation =
        predicate<R, T, T> &&
        predicate<R, U, U> &&
        predicate<R, T, U> &&
        predicate<R, U, T>;

    template<typename R, typename T, typename U>
    CPP_concept_bool strict_weak_order =
        relation<R, T, U>;
    // clang-format on

    namespace defer
    {
        template<typename Fun, typename... Args>
        CPP_concept invocable = CPP_defer_(ranges::invocable, CPP_type(Fun), Args...);

        template<typename Fun, typename... Args>
        CPP_concept regular_invocable = CPP_defer_(ranges::regular_invocable,
                                                   CPP_type(Fun), Args...);

        template<typename Fun, typename... Args>
        CPP_concept predicate = CPP_defer_(ranges::predicate, CPP_type(Fun), Args...);

        template<typename R, typename T, typename U>
        CPP_concept relation = CPP_defer(ranges::relation, R, T, U);

        template<typename R, typename T, typename U>
        CPP_concept strict_weak_order = CPP_defer(ranges::strict_weak_order, R, T, U);
    } // namespace defer

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

#include <range/v3/detail/reenable_warnings.hpp>

#endif
