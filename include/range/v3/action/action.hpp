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
#include <range/v3/range_concepts.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct action_access
            {
                template<typename Action>
                struct impl
                {
                    template<typename...Ts, typename A = Action>
                    static auto bind(Ts &&...ts)
                    RANGES_DECLTYPE_AUTO_RETURN
                    (
                        A::bind(static_cast<Ts &&>(ts)...)
                    )
                };
            };

            struct make_action_fn
            {
                template<typename Fun>
                action<Fun> operator()(Fun fun) const
                {
                    return {detail::move(fun)};
                }
            };

            /// \ingroup group-actions
            /// \relates make_action_fn
            RANGES_INLINE_VARIABLE(make_action_fn, make_action)

            CONCEPT_def
            (
                template(typename Action, typename Rng, typename...Rest)
                (concept ActionConcept)(Action, Rng, Rest...),
                    Range<Rng> &&
                    Invocable<Action const&, Rng, Rest...>
            );

            CONCEPT_def
            (
                template(typename Action, typename Rng)
                concept ActionPipeConcept,
                    Range<Rng> &&
                    Invocable<Action &, Rng> &&
                    !std::is_reference<Rng>::value
            );

            template<typename Action>
            struct action : pipeable<action<Action>>
            {
            private:
                Action action_;
                friend pipeable_access;

                // Piping requires things are passed by value.
                template<typename Rng, typename Act>
                static auto pipe(Rng &&rng, Act &&act) ->
                    CONCEPT_return_type(invoke_result_t<Action &, Rng>)(
                        requires ActionPipeConcept<Action, Rng>)
                {
                    return invoke(act.action_, detail::move(rng));
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                // For better error messages:
                template<typename Rng, typename Act>
                static auto pipe(Rng &&, Act &&) ->
                    CONCEPT_return_type(void)(
                        requires not ActionPipeConcept<Action, Rng>)
                {
                    CONCEPT_assert_msg(Range<Rng>,
                        "The type Rng must be a model of the Range concept.");
                    // BUGBUG This isn't a very helpful message. This is probably the wrong place
                    // to put this check:
                    CONCEPT_assert_msg(Invocable<Action&, Rng>,
                        "This action is not callable with this range type.");
                    static_assert(!std::is_reference<Rng>(),
                        "You can't pipe an lvalue into an action. Try using std::move on the argument, "
                        "and be sure to save the result somewhere or pipe the result to another action. "
                        "Or, wrap the argument with std::ref to pass it by reference.");
                }
            #endif

            public:
                action() = default;
                action(Action a)
                  : action_(detail::move(a))
                {}

                // Calling directly requires things are passed by reference.
                template<typename Rng, typename ...Rest>
                auto operator()(Rng &rng, Rest &&... rest) const ->
                    CONCEPT_return_type(invoke_result_t<Action const &, Rng &, Rest...>)(
                        requires ActionConcept<Action const, Rng &, Rest...>)
                {
                    return invoke(action_, rng, static_cast<Rest &&>(rest)...);
                }

                // Currying overload.
                template<typename T, typename... Rest, typename A = Action>
                auto operator()(T &&t, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_action(
                        action_access::impl<A>::bind(
                            action_,
                            static_cast<T &&>(t),
                            static_cast<Rest &&>(rest)...))
                )
            };

            template<typename Rng, typename Action>
            auto operator|=(Rng &rng, Action &&action) ->
                CONCEPT_return_type(Rng &)(
                    requires is_pipeable<Action>::value && Range<Rng &> &&
                    Invocable<bitwise_or, ref_t<Rng &>, Action &> &&
                    Same<ref_t<Rng &>,
                        invoke_result_t<bitwise_or, ref_t<Rng &>, Action &>>)
            {
                ref(rng) | action;
                return rng;
            }
        }
        /// @}
    }
}

#endif
