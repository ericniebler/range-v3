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

#ifndef RANGES_V3_CONTAINER_ACTION_HPP
#define RANGES_V3_CONTAINER_ACTION_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/concepts.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/ref.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct action_access
        {
            template<typename Action>
            struct impl
            {
                // clang-format off
                template<typename... Ts, typename A = Action>
                static constexpr auto CPP_auto_fun(bind)(Ts &&... ts)
                (
                    return A::bind(static_cast<Ts &&>(ts)...)
                )
                // clang-format on
            };
        };

        struct make_action_fn
        {
            template<typename Fun>
            constexpr action<Fun> operator()(Fun fun) const
            {
                return action<Fun>{detail::move(fun)};
            }
        };

        /// \ingroup group-actions
        /// \relates make_action_fn
        RANGES_INLINE_VARIABLE(make_action_fn, make_action)

        template<typename Action>
        struct action : pipeable_base
        {
        private:
            Action action_;
            friend pipeable_access;

            // Piping requires things are passed by value.
            template<typename Rng, typename Act>
            static auto pipe(Rng && rng, Act && act)
                -> CPP_ret(invoke_result_t<Action &, Rng>)( //
                    requires range<Rng> && invocable<Action &, Rng> &&
                    (!std::is_reference<Rng>::value))
            {
                return invoke(act.action_, detail::move(rng));
            }

        public:
            action() = default;

            constexpr explicit action(Action a) noexcept(
                std::is_nothrow_move_constructible<Action>::value)
              : action_(detail::move(a))
            {}

            // Calling directly requires things are passed by reference.
            template<typename Rng, typename... Rest>
            auto operator()(Rng & rng, Rest &&... rest) const
                -> CPP_ret(invoke_result_t<Action const &, Rng &, Rest...>)( //
                    requires range<Rng> && invocable<Action const &, Rng &, Rest...>)
            {
                return invoke(action_, rng, static_cast<Rest &&>(rest)...);
            }

            // Currying overload.
            // clang-format off
            template<typename T, typename... Rest, typename A = Action>
            auto CPP_auto_fun(operator())(T &&t, Rest &&... rest)(const)
            (
                return make_action(
                    action_access::impl<A>::bind(action_,
                                                 static_cast<T &&>(t),
                                                 static_cast<Rest &&>(rest)...))
            )
            // clang-format on
        };

        template<typename Rng, typename Action>
        auto operator|=(Rng & rng, Action && action) -> CPP_ret(Rng &)( //
            requires is_pipeable<Action>::value && range<Rng &> &&
                invocable<bitwise_or, ref_view<Rng>, Action &> && same_as<
                    ref_view<Rng>, invoke_result_t<bitwise_or, ref_view<Rng>, Action &>>)
        {
            views::ref(rng) | action;
            return rng;
        }
    } // namespace actions
    /// @}
} // namespace ranges

#endif
