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

#ifndef RANGES_V3_UTILITY_PIPEABLE_HPP
#define RANGES_V3_UTILITY_PIPEABLE_HPP

#include <functional>
#include <utility>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Bind>
            struct pipeable_binder
              : Bind
              , pipeable<pipeable_binder<Bind>>
            {
                pipeable_binder(Bind bind)
                  : Bind(std::move(bind))
                {}
            };

            template<typename Bind>
            pipeable_binder<Bind> make_pipeable_binder(Bind bind)
            {
                return {std::move(bind)};
            }
        }

        struct pipeable_bind_fn
        {
            template<typename Fn, typename ...As>
            auto operator()(Fn && fn, As &&...as) const ->
                decltype(detail::make_pipeable_binder(std::bind(std::forward<Fn>(fn), std::forward<As>(as)...)))
            {
                return detail::make_pipeable_binder(std::bind(std::forward<Fn>(fn), std::forward<As>(as)...));
            }
        };

        RANGES_CONSTEXPR pipeable_bind_fn pipeable_bind{};

        template<typename T,
            typename U = detail::conditional_t<
                std::is_lvalue_reference<T>::value,
                std::reference_wrapper<meta_apply<std::remove_reference, T>>,
                T &&>>
        U bind_forward(meta_apply<std::remove_reference, T> & t) noexcept
        {
            return static_cast<U>(t);
        }

        template<typename T>
        T && bind_forward(meta_apply<std::remove_reference, T> && t) noexcept
        {
            // This is to catch way sketchy stuff like: forward<int const &>(42)
            static_assert(!std::is_lvalue_reference<T>::value, "You didn't just do that!");
            return static_cast<T &&>(t);
        }

        struct bitwise_or
        {
            template<typename T, typename U>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t | (U &&) u)
            {
                return (T &&) t | (U &&) u;
            }
        };

        struct pipeable_base
        {};

        template<typename T>
        struct is_pipeable
          : std::is_base_of<pipeable_base, T>
        {};

        template<typename T>
        struct is_pipeable<T &>
          : is_pipeable<T>
        {};

        template<typename Derived>
        struct pipeable : pipeable_base
        {
        protected:
            // Default Pipe behavior just passes the argument to the pipe's function call
            // operator
            template<typename Arg, typename Pipe>
            static auto pipe(Arg && arg, Pipe && pipe) ->
                decltype(std::declval<Pipe>()(std::declval<Arg>()))
            {
                return std::forward<Pipe>(pipe)(std::forward<Arg>(arg));
            }
        public:
            // Evaluate the pipe with an argument
            template<typename Arg, typename D = Derived, CONCEPT_REQUIRES_(!is_pipeable<Arg>())>
            friend auto operator|(Arg && arg, pipeable const & pipe)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                D::pipe(std::forward<Arg>(arg), static_cast<D const &>(pipe))
            )
            template<typename Arg, typename D = Derived, CONCEPT_REQUIRES_(!is_pipeable<Arg>())>
            friend auto operator|(Arg && arg, pipeable && pipe)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                D::pipe(std::forward<Arg>(arg), static_cast<D &&>(pipe))
            )

            // Compose two pipes
            template<typename Arg, typename D = Derived, CONCEPT_REQUIRES_(is_pipeable<Arg>())>
            friend auto operator|(Arg && arg, pipeable const & pipe)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pipeable_bind(
                    bitwise_or{},
                    std::bind(bitwise_or{}, std::placeholders::_1, bind_forward<Arg>(arg)),
                    std::ref(static_cast<D const &>(pipe))
                )
            )
            template<typename Arg, typename D = Derived, CONCEPT_REQUIRES_(is_pipeable<Arg>())>
            friend auto operator|(Arg && arg, pipeable && pipe)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pipeable_bind(
                    bitwise_or{},
                    std::bind(bitwise_or{}, std::placeholders::_1, bind_forward<Arg>(arg)),
                    static_cast<D &&>(pipe)
                )
            )
        };
    }
}

#endif // RANGES_V3_UTILITY_PIPEABLE_HPP
