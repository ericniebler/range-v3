// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_INVOKABLE_HPP
#define RANGES_V3_UTILITY_INVOKABLE_HPP

#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
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

        /// \ingroup group-utility
        /// \sa `make_invokable_fn`
        constexpr make_invokable_fn invokable {};

        template<typename T>
        using invokable_t = decltype(invokable(std::declval<T>()));

        template<typename Fun>
        using semiregular_invokable_t =
            meta::if_<
                SemiRegular<invokable_t<Fun>>,
                invokable_t<Fun>,
                semiregular<invokable_t<Fun>>>;

        template<typename Fun, bool IsConst = false>
        using semiregular_invokable_ref_t =
            meta::if_<
                SemiRegular<invokable_t<Fun>>,
                invokable_t<Fun>,
                reference_wrapper<
                    meta::apply<meta::add_const_if_c<IsConst>, semiregular<invokable_t<Fun>>>>>;
        /// @}

        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct Invokable
            {
                template<typename Fun, typename...Args>
                using result_t = Function::result_t<invokable_t<Fun>, Args...>;

                template<typename Fun, typename...Args>
                auto requires_(Fun, Args...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Function, invokable_t<Fun>, Args...>()
                    ));
            };

            struct RegularInvokable
              : refines<Invokable>
            {};

            struct InvokablePredicate
              : refines<RegularInvokable>
            {
                template<typename Fun, typename...Args>
                auto requires_(Fun, Args...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate, invokable_t<Fun>, Args...>()
                    ));
            };

            struct InvokableRelation
              : refines<InvokablePredicate>
            {
                template<typename Fun, typename T, typename U>
                auto requires_(Fun, T, U) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Relation, invokable_t<Fun>, T, U>()
                    ));
            };

            struct InvokableTransform
              : refines<RegularInvokable>
            {
                template<typename Fun, typename T>
                auto requires_(Fun, T) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Transform, invokable_t<Fun>, T>()
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
        /// @}
    }
}

#endif // RANGES_V3_UTILITY_INVOKABLE_HPP
