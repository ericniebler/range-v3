// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_INVOKABLE_HPP
#define RANGES_V3_UTILITY_INVOKABLE_HPP

#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct make_invokable_fn
        {
            template<typename R, typename T>
            auto operator()(R T::* p) const -> decltype(std::mem_fn(p))
            {
                return std::mem_fn(p);
            }

            template<typename T, typename U = detail::decay_t<T>>
            auto operator()(T && t) const -> enable_if_t<!std::is_member_pointer<U>::value, T>
            {
                return std::forward<T>(t);
            }
        };

        RANGES_CONSTEXPR make_invokable_fn invokable {};

        template<typename T>
        using invokable_t = decltype(ranges::invokable(std::declval<T>()));

        namespace concepts
        {
            struct Invokable
            {
                template<typename Fun, typename...Args>
                using result_t = Function::result_t<invokable_t<Fun>, Args...>;

                template<typename Fun, typename...Args>
                auto requires(Fun &&fun, Args &&...args) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Function>(ranges::invokable((Fun &&) fun), (Args &&) args...)
                    ));
            };

            struct RegularInvokable
              : refines<Invokable>
            {};

            struct InvokablePredicate
              : refines<RegularInvokable>
            {
                template<typename Fun, typename...Args>
                auto requires(Fun &&fun, Args &&...args) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>(ranges::invokable((Fun &&) fun), (Args &&) args...)
                    ));
            };

            struct InvokableRelation
              : refines<InvokablePredicate>
            {
                template<typename Fun, typename T, typename U>
                auto requires(Fun &&fun, T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Relation>(ranges::invokable((Fun &&) fun), (T &&) t, (U &&) u)
                    ));
            };

            struct InvokableTransform
              : refines<RegularInvokable>
            {
                template<typename Fun, typename T>
                auto requires(Fun &&fun, T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Transform>(ranges::invokable((Fun &&) fun), (T &&) t)
                    ));
            };
        }

        template<typename Fun, typename...Args>
        using Invokable = concepts::models<concepts::Invokable, Fun, Args...>;

        template<typename Fun, typename...Args>
        using RegularInvokable = concepts::models<concepts::RegularInvokable, Fun, Args...>;

        template<typename Fun, typename...Args>
        using InvokablePredicate = concepts::models<concepts::InvokablePredicate, Fun, Args...>;

        template<typename Fun, typename T, typename U = T>
        using InvokableRelation = concepts::models<concepts::InvokableRelation, Fun, T, U>;

        template<typename F, typename T>
        using InvokableTransform = concepts::models<concepts::InvokableTransform, F, T>;
    }
}

#endif // RANGES_V3_UTILITY_INVOKABLE_HPP
