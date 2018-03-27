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
                        A::bind(static_cast<Ts&&>(ts)...)
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

            template<typename Action>
            struct action : pipeable<action<Action>>
            {
            private:
                Action action_;
                friend pipeable_access;

                template<typename Rng, typename...Rest>
                using ActionConcept = meta::and_<
                    Range<Rng>,
                    Invocable<Action const&, Rng, Rest...>>;

                template<typename Rng>
                using ActionPipeConcept = meta::and_<
                    Invocable<Action&, Rng>,
                    Range<Rng>,
                    meta::not_<std::is_reference<Rng>>>;

                // Piping requires things are passed by value.
                template<typename Rng, typename Act,
                    CONCEPT_REQUIRES_(ActionPipeConcept<Rng>())>
                static auto pipe(Rng && rng, Act && act)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    invoke(act.action_, detail::move(rng))
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                // For better error messages:
                template<typename Rng, typename Act,
                    CONCEPT_REQUIRES_(!ActionPipeConcept<Rng>())>
                static void pipe(Rng &&, Act &&)
                {
                    CONCEPT_ASSERT_MSG(Range<Rng>(),
                        "The type Rng must be a model of the Range concept.");
                    // BUGBUG This isn't a very helpful message. This is probably the wrong place
                    // to put this check:
                    CONCEPT_ASSERT_MSG(Invocable<Action&, Rng>(),
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
                template<typename Rng, typename...Rest,
                    CONCEPT_REQUIRES_(ActionConcept<Rng &, Rest...>())>
                auto operator()(Rng & rng, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    invoke(action_, rng, static_cast<Rest&&>(rest)...)
                )

                // Currying overload.
                template<typename T, typename...Rest, typename A = Action>
                auto operator()(T && t, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_action(action_access::impl<A>::bind(action_, static_cast<T&&>(t),
                        static_cast<Rest&&>(rest)...))
                )
            };

            template<typename Rng, typename Action,
                CONCEPT_REQUIRES_(is_pipeable<Action>() && Range<Rng &>() &&
                    Invocable<bitwise_or, ref_t<Rng &>, Action &>() &&
                    Same<ref_t<Rng &>,
                        invoke_result_t<bitwise_or, ref_t<Rng &>, Action &>>())>
            Rng & operator|=(Rng & rng, Action && action)
            {
                ref(rng) | action;
                return rng;
            }
        }
        /// @}
    }
}

#endif
