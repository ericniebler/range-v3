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
            struct bind_wrapper
            {
            private:
                friend struct unwrap_binder;
                Bind bind_; // not a base class to avoid ADL on std namespace
            public:
                bind_wrapper(Bind bind)
                  : bind_(detail::move(bind))
                {}
                template<typename ...Args,
                    CONCEPT_REQUIRES(False<is_bind_expression<Args...>>())>
                auto operator()(Args &&...args) const
                    -> decltype(std::declval<Bind const &>()(detail::forward<Args>(args)...))
                {
                    return bind_(detail::forward<Args>(args)...);
                }
                template<typename Arg,
                    CONCEPT_REQUIRES(False<is_bind_expression<Arg>>())>
                friend auto operator|(Arg && arg, bind_wrapper const & bind)
                    -> decltype(std::declval<Bind const &>()(detail::forward<Arg>(arg)))
                {
                    return bind.bind_(detail::forward<Arg>(arg));
                }
            };

            constexpr struct binder
            {
                template<typename ...Args>
                auto operator()(Args &&... args) const
                    -> bind_wrapper<decltype(std::bind(detail::forward<Args>(args)...))>
                {
                    // BUGBUG std::bind doesn't do perfect forwarding. It will store
                    // a copy of all arguments unless I use reference_wrapper here.
                    // Write a version of forward<>() that uses reference_wrapper and
                    // use it!
                    return {std::bind(detail::forward<Args>(args)...)};
                }
            } bind {};

            template<typename T>
            false_ is_binder_(T const &);

            template<typename T>
            true_ is_binder_(bind_wrapper<T> const &);

            template<typename T>
            struct is_binder
              : decltype(detail::is_binder_(std::declval<T>()))
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
                    CONCEPT_REQUIRES(False<is_binder<T>>())>
                T && operator()(T && t) const
                {
                    return detail::forward<T>(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES(True<is_binder<T>>())>
                auto operator()(T && t) const -> decltype((detail::forward<T>(t).bind_))
                {
                    return detail::forward<T>(t).bind_;
                }
            } unwrap_bind {};
        }

        template<typename ...T>
        struct is_bind_expression
          : detail::or_<(detail::is_binder<T>::value || detail::is_placeholder<T>::value)...>
        {};

        template<typename Fun>
        struct bindable : Fun
        {
        private:
            constexpr Fun const & fun() const &
            {
                return *this;
            }
            constexpr Fun && fun() const &&
            {
                return const_cast<bindable &&>(*this);
            }
        public:
            bindable() = default;

            explicit bindable(Fun fun)
              : Fun(detail::move(fun))
            {}
            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename ...Args,
                CONCEPT_REQUIRES(True<is_bind_expression<Args...>>())>
            constexpr auto operator()(Args &&... args) const &
                -> decltype(detail::bind(std::declval<Fun const &>(),
                                         detail::unwrap_bind(detail::forward<Args>(args))...))
            {
                return detail::bind(fun(),
                                    detail::unwrap_bind(detail::forward<Args>(args))...);
            }
            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename ...Args,
                CONCEPT_REQUIRES(True<is_bind_expression<Args...>>())>
            constexpr auto operator()(Args &&... args) const &&
                -> decltype(detail::bind(std::declval<Fun &&>(),
                                         detail::unwrap_bind(detail::forward<Args>(args))...))
            {
                return detail::bind(detail::move(*this).fun(),
                                    detail::unwrap_bind(detail::forward<Args>(args))...);
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename ...Args,
                CONCEPT_REQUIRES(False<is_bind_expression<Args...>>())>
            constexpr auto operator()(Args &&... args) const &
                -> decltype(std::declval<Fun const &>()(detail::forward<Args>(args)...))
            {
                return fun()(detail::forward<Args>(args)...);
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename ...Args,
                CONCEPT_REQUIRES(False<is_bind_expression<Args...>>())>
            constexpr auto operator()(Args &&... args) const &&
                -> decltype(std::declval<Fun &&>()(detail::forward<Args>(args)...))
            {
                return detail::move(*this).fun()(detail::forward<Args>(args)...);
            }

            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename Arg,
                CONCEPT_REQUIRES(True<is_bind_expression<Arg>>())>
            constexpr friend auto operator|(Arg && arg, bindable const & bind)
                -> decltype(detail::bind(std::declval<Fun const &>(),
                                         detail::unwrap_bind(detail::forward<Arg>(arg))))
            {
                return detail::bind(bind.fun(),
                                    detail::unwrap_bind(detail::forward<Arg>(arg)));
            }
            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename Arg,
                CONCEPT_REQUIRES(True<is_bind_expression<Arg>>())>
            constexpr friend auto operator|(Arg && arg, bindable && bind)
                -> decltype(detail::bind(std::declval<Fun &&>(),
                                         detail::unwrap_bind(detail::forward<Arg>(arg))))
            {
                return detail::bind(detail::move(bind).fun(),
                                    detail::unwrap_bind(detail::forward<Arg>(arg)));
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename Arg,
                CONCEPT_REQUIRES(False<is_bind_expression<Arg>>())>
            constexpr friend auto operator|(Arg && arg, bindable const & bind)
                -> decltype(std::declval<Fun const &>()(detail::forward<Arg>(arg)))
            {
                return bind.fun()(detail::forward<Arg>(arg));
            }
            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename Arg,
                CONCEPT_REQUIRES(False<is_bind_expression<Arg>>())>
            constexpr friend auto operator|(Arg && arg, bindable && bind)
                -> decltype(std::declval<Fun &&>()(detail::forward<Arg>(arg)))
            {
                return detail::move(bind).fun()(detail::forward<Arg>(arg));
            }
        };
    }
}

#endif // RANGES_V3_UTILITY_BINDABLE_HPP
