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

        RANGES_CONSTEXPR make_invokable_fn make_invokable {};

        namespace concepts
        {
            struct Invokable
            {
                template<typename T, typename...Args>
                using result_t = decltype(ranges::make_invokable(std::declval<T>())(std::declval<Args>()...));

                template<typename T, typename...Args>
                auto requires(T &&t, Args &&...args) -> decltype(
                    concepts::valid_expr(
                        (ranges::make_invokable((T &&) t)((Args &&) args...), 42)
                    ));
            };

            struct InvokablePredicate
              : refines<Invokable>
            {
                template<typename T, typename...Args>
                auto requires(T &&t, Args &&...args) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(
                            ranges::make_invokable((T &&) t)((Args &&) args...))
                    ));
            };
        }

        template<typename T, typename...Args>
        using Invokable = concepts::models<concepts::Invokable, T, Args...>;

        template<typename T, typename...Args>
        using InvokablePredicate = concepts::models<concepts::InvokablePredicate, T, Args...>;
    }
}

#endif // RANGES_V3_UTILITY_INVOKABLE_HPP
