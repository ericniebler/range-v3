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
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/ref.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    struct make_action_fn
    {
        template<typename Fun>
        constexpr actions::action<Fun> operator()(Fun fun) const
        {
            return actions::action<Fun>{detail::move(fun)};
        }
    };

    /// \ingroup group-actions
    /// \relates make_action_fn
    RANGES_INLINE_VARIABLE(make_action_fn, make_action)

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

        using ranges::make_action; // for legacy reasons

        struct action_base
        {
        private:
            // clang-format off
            template<typename Act1, typename Act2>
            struct composed
            {
                Act1 act1_;
                Act2 act2_;
                template<typename Arg>
                constexpr auto CPP_auto_fun(operator())(Arg && arg) (const)
                (
                    return static_cast<Arg &&>(arg) | act1_ | act2_
                )
            };
            // clang-format on
        public:
            template<typename Act>
            constexpr static Act && get_action(action<Act> && act) noexcept
            {
                return static_cast<Act &&>(act.action_);
            }

            // Piping requires things are passed by value.
            CPP_template(typename Rng, typename Act)(                       //
                requires defer::range<Rng> && defer::invocable<Act, Rng> && //
                (!defer::is_true<std::is_reference<Rng>::value>))           //
                constexpr friend decltype(auto)
                operator|(Rng && rng, action<Act> act)
            {
                return get_action(static_cast<action<Act> &&>(act))(
                    static_cast<Rng &&>(rng));
            }

            // This overload is deleted because when piping a range into an
            // action, it must be moved in.
            template<typename Rng, typename Act>
            // RANGES_DEPRECATED("You must pipe an rvalue range into an action.")
            constexpr friend auto operator|(Rng &&, action<Act> const &) -> CPP_ret(Rng)(
                requires range<Rng> && std::is_reference<Rng>::value) = delete;

            template<typename Act1, typename Act2>
            constexpr friend auto operator|(action<Act1> act1, action<Act2> act2)
            {
                return make_action(composed<action<Act1>, action<Act2>>{
                    static_cast<action<Act1> &&>(act1),
                    static_cast<action<Act2> &&>(act2)});
            }
        };

        template<typename Action>
        struct action : action_base
        {
        private:
            Action action_;
            friend action_base;

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
            CPP_template(typename... Rest, typename A = Action)(
                requires(sizeof...(Rest) != 0))
            auto CPP_auto_fun(operator())(Rest &&... rest)(const)
            (
                return make_action(
                    action_access::impl<A>::bind(action_,
                                                 static_cast<Rest &&>(rest)...))
            )
            // clang-format on
        };

        template<typename Rng, typename Act>
        auto operator|=(Rng & rng, action<Act> act) -> CPP_ret(Rng &)( //
            requires range<Rng &> && invocable<bitwise_or, ref_view<Rng>, action<Act>> &&
                same_as<ref_view<Rng>,
                        invoke_result_t<bitwise_or, ref_view<Rng>, action<Act>>>)
        {
            views::ref(rng) | static_cast<action<Act> &&>(act);
            return rng;
        }
    } // namespace actions

    template<typename Act>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<actions::action<Act>> = true;
    /// @}
} // namespace ranges

#endif
