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
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T>
            using uncvref_t =
                typename std::remove_cv<typename std::remove_reference<T>::type>::type;

            template<typename Bind>
            struct bind_wrapper : Bind, pipeable<bind_wrapper<Bind>>
            {
                Bind const & bind() const & { return *this; }
                Bind && bind() && { return static_cast<Bind &&>(*this); }

                bind_wrapper(Bind bind)
                  : Bind(detail::move(bind))
                {}
            };

            template<typename...Args>
            using std_bind_t = decltype(std::bind(std::declval<Args>()...));

            constexpr struct binder
            {
                template<typename ...Args>
                bind_wrapper<std_bind_t<Args...>> operator()(Args &&... args) const
                {
                    // BUGBUG std::bind doesn't do perfect forwarding. It will store
                    // a copy of all arguments unless I use reference_wrapper here.
                    // Write a version of forward<>() that uses reference_wrapper and
                    // use it!
                    return {std::bind(detail::forward<Args>(args)...)};
                }
            } bind {};

            template<typename...Args>
            using bind_t = decltype(detail::bind(std::declval<Args>()...));

            template<typename T>
            false_ is_bind_wrapper_(T const &);

            template<typename T>
            true_ is_bind_wrapper_(bind_wrapper<T> const &);

            template<typename T>
            struct is_bind_wrapper
              : decltype(detail::is_bind_wrapper_(std::declval<T>()))
            {};

            template<typename ...T>
            struct is_placeholder
              : detail::or_<is_placeholder<T>::value...>
            {};

            template<typename T>
            struct is_placeholder<T>
              : std::is_placeholder<uncvref_t<T>>::type
            {};

            constexpr struct unwrap_binder
            {
                template<typename T,
                    CONCEPT_REQUIRES(False<is_bind_wrapper<T>>())>
                T && operator()(T && t) const
                {
                    return detail::forward<T>(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES(True<is_bind_wrapper<T>>())>
                auto operator()(T && t) const -> decltype(std::declval<T>().bind())
                {
                    return detail::forward<T>(t).bind();
                }
            } unwrap_bind {};

            template<typename T>
            using unwrap_bind_t = decltype(detail::unwrap_bind(std::declval<T>()));
        }

        template<typename ...T>
        struct is_bind_expression
          : detail::or_<(detail::is_bind_wrapper<T>::value || detail::is_placeholder<T>::value)...>
        {};

        template<typename Derived>
        struct bindable
        {
        private:
            Derived const & derived() const &
            {
                return static_cast<Derived const &>(*this);
            }
            Derived && derived() const &&
            {
                return static_cast<Derived &&>(const_cast<bindable &&>(*this));
            }

        public:
            ////////////////////////////////////////////////////////////////////////////////////////
            // operator()
            ////////////////////////////////////////////////////////////////////////////////////////

            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename ...Args,
                CONCEPT_REQUIRES(True<is_bind_expression<Args...>>())>
            auto operator()(Args &&... args) const &
                -> detail::bind_t<Derived const &, detail::unwrap_bind_t<Args>...>
            {
                return detail::bind(derived(),
                                    detail::unwrap_bind(detail::forward<Args>(args))...);
            }
            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename ...Args,
                CONCEPT_REQUIRES(True<is_bind_expression<Args...>>())>
            auto operator()(Args &&... args) const &&
                -> detail::bind_t<Derived &&, detail::unwrap_bind_t<Args>...>
            {
                return detail::bind(detail::move(*this).derived(),
                                    detail::unwrap_bind(detail::forward<Args>(args))...);
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename ...Args,
                CONCEPT_REQUIRES(False<is_bind_expression<Args...>>())>
            auto operator()(Args &&... args) const &
            -> decltype(detail::always_t<Derived, Args...>::invoke(std::declval<Derived const &>(), std::declval<Args>()...))
            {
                return Derived::invoke(derived(), detail::forward<Args>(args)...);
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename ...Args,
                CONCEPT_REQUIRES(False<is_bind_expression<Args...>>())>
            auto operator()(Args &&... args) const &&
                -> decltype(detail::always_t<Derived, Args...>::invoke(std::declval<Derived &&>(), std::declval<Args>()...))
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
            Derived && derived() const &&
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
            ////////////////////////////////////////////////////////////////////////////////////////
            // operator|
            ////////////////////////////////////////////////////////////////////////////////////////

            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename Arg>
            friend auto operator|(Arg && arg, pipeable const & pipe)
                -> decltype(detail::always_t<Derived, Arg>::pipe(std::declval<Arg>(), std::declval<Derived const &>()))
            {
                return Derived::pipe(detail::forward<Arg>(arg), pipe.derived());
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
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
