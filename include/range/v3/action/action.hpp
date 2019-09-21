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
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/ref.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    struct make_action_closure_fn
    {
        template<typename Fun>
        constexpr actions::action_closure<Fun> operator()(Fun fun) const
        {
            return actions::action_closure<Fun>{static_cast<Fun &&>(fun)};
        }
    };

    /// \sa make_view_fn
    RANGES_INLINE_VARIABLE(make_action_closure_fn, make_action_closure)

    namespace actions
    {
        struct action_closure_base
        {
        private:
            template<typename ActionFn1, typename ActionFn2>
            struct composed
            {
                ActionFn1 act1_;
                ActionFn2 act2_;
                template<typename Arg>
                constexpr decltype(auto) CPP_fun(operator())(Arg && arg)(
                    const //
                    requires invocable<ActionFn1 const &, Arg &> && invocable<
                        ActionFn2 const &, invoke_result_t<ActionFn1 const &, Arg &>>)
                {
                    return act2_(act1_(arg));
                }
            };

        public:
            // Piping requires things are passed by value.
            CPP_template(typename Rng, typename ActionFn)(                         //
                requires defer::range<Rng> && defer::invocable<ActionFn, Rng &> && //
                (!defer::is_true<std::is_reference<Rng>::value>))                  //
                constexpr friend decltype(auto)
                operator|(Rng && rng, action_closure<ActionFn> act)
            {
                using Ret = invoke_result_t<ActionFn, Rng &>;
                return static_cast<aux::move_t<Ret>>(static_cast<ActionFn &&>(act)(rng));
            }

            template<typename Rng, typename ActionFn>   // ******************************
            constexpr friend auto                       // ******************************
            operator|(Rng &,                            // ********* READ THIS **********
                      action_closure<ActionFn> const &) // ****** IF YOUR COMPILE *******
                -> CPP_ret(Rng)(                        // ******** BREAKS HERE *********
                    requires range<Rng>) = delete;      // ******************************
            // **************************************************************************
            // *    When piping a range into an action, the range must be a be moved    *
            // *    in.                                                                 *
            // **************************************************************************

            template<typename ActionFn1, typename ActionFn2>
            constexpr friend auto operator|(action_closure<ActionFn1> act1,
                                            action_closure<ActionFn2> act2)
            {
                return make_action_closure(
                    composed<action_closure<ActionFn1>, action_closure<ActionFn2>>{
                        static_cast<action_closure<ActionFn1> &&>(act1),
                        static_cast<action_closure<ActionFn2> &&>(act2)});
            }

            template<typename Rng, typename ActionFn>
            friend constexpr auto operator|=(Rng & rng, action_closure<ActionFn> act) //
                -> CPP_ret(Rng &)(                                                    //
                    requires range<Rng> && invocable<ActionFn, Rng &>)
            {
                static_cast<ActionFn &&>(act)(rng);
                return rng;
            }
        };

        template<typename ActionFn>
        struct RANGES_EMPTY_BASES action_closure
          : action_closure_base
          , ActionFn
        {
            action_closure() = default;

            constexpr explicit action_closure(ActionFn fn)
              : ActionFn(static_cast<ActionFn &&>(fn))
            {}

            using ActionFn::operator();
        };

        /// \cond
        /// DEPRECATED STUFF
        struct action_access_
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

        using action_access RANGES_DEPRECATED(
            "action_access and actions::action<> are deprecated. Please "
            "replace action<> with action_closure<> and discontinue use of "
            "action_access.") = action_access_;

        template<typename>
        struct action_;

        struct make_action_fn_
        {
            template<typename Fun>
            constexpr action_<Fun> operator()(Fun fun) const
            {
                return action_<Fun>{static_cast<Fun &&>(fun)};
            }
        };
        using make_action_fn RANGES_DEPRECATED(
            "make_action_fn is deprecated. Please use "
            "make_action_closure instead.") = make_action_fn_;

        namespace
        {
            RANGES_DEPRECATED(
                "make_action and actions::action<> has been deprecated. Please switch to "
                "make_action_closure and action::action_closure.")
            RANGES_INLINE_VAR constexpr auto & make_action =
                static_const<make_action_fn_>::value;
        } // namespace

        template<typename Action>
        struct action_ : pipeable_base
        {
        private:
            Action act_;
            friend pipeable_access;

        public:
            action_() = default;

            constexpr explicit action_(Action a) noexcept(
                std::is_nothrow_move_constructible<Action>::value)
              : act_(detail::move(a))
            {}

            // Calling directly requires things are passed by reference.
            template<typename Rng, typename... Rest>
            auto operator()(Rng & rng, Rest &&... rest) const
                -> CPP_ret(invoke_result_t<Action const &, Rng &, Rest...>)( //
                    requires range<Rng> && invocable<Action const &, Rng &, Rest...>)
            {
                return invoke(act_, rng, static_cast<Rest &&>(rest)...);
            }

            // Currying overload.
            // clang-format off
            CPP_template(typename... Rest, typename A = Action)(
                requires(sizeof...(Rest) != 0))
            auto CPP_auto_fun(operator())(Rest &&... rest)(const)
            (
                return make_action_fn_{}(
                    action_access_::impl<A>::bind(act_,
                                                  static_cast<Rest &&>(rest)...))
            )
            // clang-format on
        };

        template<typename Rng, typename ActionFn>
        auto operator|=(Rng & rng, action_<ActionFn> act) -> CPP_ret(Rng &)( //
            requires range<Rng &> && invocable<bitwise_or, ref_view<Rng>,
                                               action_<ActionFn>> &&
                same_as<ref_view<Rng>,
                        invoke_result_t<bitwise_or, ref_view<Rng>, action_<ActionFn>>>)
        {
            views::ref(rng) | static_cast<action_<ActionFn> &&>(act);
            return rng;
        }

        template<typename Action>
        using action RANGES_DEPRECATED(
            "The actions::action<> template is deprecated. Please switch to "
            "action_closure") = action_<Action>;
        /// \endcond
    } // namespace actions

    template<typename ActionFn>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<actions::action_closure<ActionFn>> =
        true;
    /// @}
} // namespace ranges

#endif
