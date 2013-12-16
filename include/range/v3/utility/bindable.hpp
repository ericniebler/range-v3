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

#ifndef RANGES_V3_UTILITY_BINDABLE_HPP
#define RANGES_V3_UTILITY_BINDABLE_HPP

#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/utility/bind.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/logical_ops.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Bind>
            struct binder_wrapper : Bind, pipeable<binder_wrapper<Bind>>
            {
                Bind & bind() & { return *this; }
                Bind const & bind() const & { return *this; }
                Bind && bind() && { return std::move(*this); }

                binder_wrapper(Bind bind)
                  : Bind(detail::move(bind))
                {}
            };

            // is_bind_wrapper
            template<typename T>
            struct is_bind_wrapper : std::false_type
            {};

            template<typename T>
            struct is_bind_wrapper<binder_wrapper<T>> : std::true_type
            {};

            // is_placeholder_expression
            template<typename T>
            struct is_placeholder_expression
              : std::integral_constant<bool, is_bind_wrapper<T>::value ||
                                             0 != std::is_placeholder<T>::value>
            {};

            template<typename...Args>
            using ranges_binder_t = decltype(ranges::bind(std::declval<Args>()...));

            RANGES_CONSTEXPR struct bind_wrapper_maker
            {
                template<typename ...Args>
                binder_wrapper<ranges_binder_t<Args...>> operator()(Args &&... args) const
                {
                    return {ranges::bind(detail::forward<Args>(args)...)};
                }
            } wrap_bind {};

            template<typename...Args>
            using bind_t = decltype(detail::wrap_bind(std::declval<Args>()...));

            RANGES_CONSTEXPR struct unwrap_binder
            {
                template<typename T,
                    CONCEPT_REQUIRES(!is_bind_wrapper<uncvref_t<T>>())>
                T && operator()(T && t) const
                {
                    return detail::forward<T>(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES(is_bind_wrapper<uncvref_t<T>>())>
                auto operator()(T && t) const -> decltype(std::declval<T>().bind())
                {
                    return detail::forward<T>(t).bind();
                }
            } unwrap_bind {};

            template<typename T>
            using unwrap_bind_t = decltype(detail::unwrap_bind(std::declval<T>()));
        }

        template<typename ...T>
        using contains_placeholder_expression =
            logical_or<detail::is_placeholder_expression<detail::uncvref_t<T>>::value...>;

        template<typename Derived>
        struct bindable
        {
        private:
            Derived const & derived() const &
            {
                return static_cast<Derived const &>(*this);
            }
            Derived && derived() &&
            {
                return static_cast<Derived &&>(*this);
            }
        public:
            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename ...Args,
                CONCEPT_REQUIRES(contains_placeholder_expression<Args...>())>
            auto operator()(Args &&... args) const &
                -> detail::bind_t<Derived const &, detail::unwrap_bind_t<Args>...>
            {
                return detail::wrap_bind(derived(),
                                         detail::unwrap_bind(detail::forward<Args>(args))...);
            }
            template<typename ...Args,
                CONCEPT_REQUIRES(contains_placeholder_expression<Args...>())>
            auto operator()(Args &&... args) &&
                -> detail::bind_t<Derived &&, detail::unwrap_bind_t<Args>...>
            {
                return detail::wrap_bind(detail::move(*this).derived(),
                                         detail::unwrap_bind(detail::forward<Args>(args))...);
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename ...Args,
                CONCEPT_REQUIRES(!contains_placeholder_expression<Args...>())>
            auto operator()(Args &&... args) const &
                -> decltype(detail::always_t<Derived, Args...>::invoke(std::declval<Derived const &>(), std::declval<Args>()...))
            {
                return Derived::invoke(derived(), detail::forward<Args>(args)...);
            }
            template<typename ...Args,
                CONCEPT_REQUIRES(!contains_placeholder_expression<Args...>())>
            auto operator()(Args &&... args) &&
                -> decltype(detail::always_t<Derived, Args...>::invoke(std::declval<Derived>(), std::declval<Args>()...))
            {
                return Derived::invoke(detail::move(*this).derived(), detail::forward<Args>(args)...);
            }
        };

        template<typename Derived>
        struct pipeable
        {
        private:
            Derived const & derived() const &
            {
                return static_cast<Derived const &>(*this);
            }
            Derived && derived() &&
            {
                return static_cast<Derived &&>(const_cast<pipeable &&>(*this));
            }

            // Default Pipe behavior just passes the argument to the pipe's function call
            // operator
            template<typename Arg, typename Pipe>
            static auto pipe(Arg && arg, Pipe && pipe)
                -> decltype(std::declval<Pipe>()(std::declval<Arg>()))
            {
                return detail::forward<Pipe>(pipe)(detail::forward<Arg>(arg));
            }
        public:
            template<typename Arg>
            friend auto operator|(Arg && arg, pipeable const & pipe)
                -> decltype(detail::always_t<Derived, Arg>::pipe(std::declval<Arg>(), std::declval<Derived const &>()))
            {
                return Derived::pipe(detail::forward<Arg>(arg), pipe.derived());
            }
            template<typename Arg>
            friend auto operator|(Arg && arg, pipeable && pipe)
                -> decltype(detail::always_t<Derived, Arg>::pipe(std::declval<Arg>(), std::declval<Derived>()))
            {
                return Derived::pipe(detail::forward<Arg>(arg), detail::move(pipe).derived());
            }
        };
    }
}

#endif // RANGES_V3_UTILITY_BINDABLE_HPP
