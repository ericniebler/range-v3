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

#ifndef RANGE_V3_UTILITY_BINDABLE_HPP
#define RANGE_V3_UTILITY_BINDABLE_HPP

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

            struct unwrap_binder;

            template<typename Bind>
            struct binder
            {
            private:
                friend struct unwrap_binder;
                Bind bind_; // not a base class to avoid ADL on std namespace
            public:
                explicit binder(Bind bind)
                  : bind_(std::move(bind))
                {}
                template<typename ...Args>
                auto operator()(Args &&...args) const
                    -> decltype(bind_(std::forward<Args>(args)...))
                {
                    return bind_(std::forward<Args>(args)...);
                }
                template<typename Arg,
                    CONCEPT_REQUIRES(False<is_bind_expression<Arg>>())>
                friend auto operator|(Arg && arg, binder const & bind)
                    -> decltype(std::declval<Bind const &>()(std::forward<Arg>(arg)))
                {
                    return bind.bind_(std::forward<Arg>(arg));
                }
            };

            constexpr struct bind_t
            {
            private:
                template<typename Bind>
                binder<Bind> impl(Bind bind) const
                {
                    return binder<Bind>{std::move(bind)};
                }
            public:
                template<typename ...Args>
                auto operator()(Args &&... args) const
                    -> decltype(this->impl(std::bind(std::forward<Args>(args)...)))
                {
                    return this->impl(std::bind(std::forward<Args>(args)...));
                }
            } bind {};

            constexpr bool or_()
            {
                return false;
            }

            template<typename ...Tail>
            constexpr bool or_(bool head, Tail...tail)
            {
                return head || detail::or_(tail...);
            }

            template<typename T>
            std::false_type is_binder_(T const &);

            template<typename T>
            std::true_type is_binder_(binder<T> const &);

            template<typename T>
            using is_binder = decltype(detail::is_binder_(std::declval<T>()));

            template<typename ...T>
            struct is_placeholder
              : std::integral_constant<
                    bool
                  , detail::or_(is_placeholder<T>::value...)
                >
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
                    return std::forward<T>(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES(True<is_binder<T>>())>
                auto operator()(T && t) const -> decltype((std::forward<T>(t).bind_))
                {
                    return std::forward<T>(t).bind_;
                }
            } unwrap_bind {};
        }

        template<typename ...T>
        struct is_bind_expression
            : std::integral_constant<
                bool
              , detail::or_((detail::is_binder<T>::value || detail::is_placeholder<T>::value)...)
            >
        {};

        template<typename Fun>
        struct bindable : Fun
        {
        private:
            constexpr Fun const & fun() const
            {
                return *this;
            }
        public:
            bindable() = default;

            bindable(Fun fun)
              : Fun(std::move(fun))
            {}

            // This gets called when one or more of the arguments are either a
            // std placeholder, or another bind expression made with bindable
            template<typename ...Args,
                CONCEPT_REQUIRES(True<is_bind_expression<Args...>>())>
            constexpr auto operator()(Args &&... args) const
                -> decltype(detail::bind(std::declval<Fun const &>(),
                                         detail::unwrap_bind(std::forward<Args>(args))...))
            {
                return detail::bind(fun(),
                                    detail::unwrap_bind(std::forward<Args>(args))...);
            }

            // This gets called when none of the arguments are std placeholders
            // or bind expressions.
            template<typename ...Args,
                CONCEPT_REQUIRES(False<is_bind_expression<Args...>>())>
            constexpr auto operator()(Args &&... args) const
                -> decltype(std::declval<Fun const &>()(std::forward<Args>(args)...))
            {
                return fun()(std::forward<Args>(args)...);
            }

            template<typename Arg,
                CONCEPT_REQUIRES(False<is_bind_expression<Arg>>())>
            friend auto operator|(Arg && arg, bindable const & bind)
                -> decltype(std::declval<Fun const &>()(std::forward<Arg>(arg)))
            {
                return bind.fun()(std::forward<Arg>(arg));
            }
        };
    }
}

#endif // RANGE_V3_UTILITY_BINDABLE_HPP
