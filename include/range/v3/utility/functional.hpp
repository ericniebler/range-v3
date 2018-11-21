/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTILITY_FUNCTIONAL_HPP
#define RANGES_V3_UTILITY_FUNCTIONAL_HPP

#include <functional> // std::reference_wrapper
#include <initializer_list>
#include <memory> // std::addressof
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/associated_types.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/invoke.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        struct equal_to
        {
            template<typename T, typename U>
            constexpr auto operator()(T &&t, U &&u) const ->
                CPP_ret(bool)(
                    requires EqualityComparableWith<T, U>)
            {
                return (T &&) t == (U &&) u;
            }
            using is_transparent = void;
        };

        struct not_equal_to
        {
            template<typename T, typename U>
            constexpr auto operator()(T &&t, U &&u) const ->
                CPP_ret(bool)(
                    requires EqualityComparableWith<T, U>)
            {
                return (T &&) t != (U &&) u;
            }
            using is_transparent = void;
        };

        struct less
        {
            template<typename T, typename U>
            constexpr auto operator()(T &&t, U &&u) const ->
                CPP_ret(bool)(
                    requires StrictTotallyOrderedWith<T, U>)
            {
                return (T &&) t < (U &&) u;
            }
            using is_transparent = void;
        };

        struct ordered_less
          : less
        {};

        struct ident
        {
            template<typename T>
            constexpr
            T &&operator()(T &&t) const noexcept
            {
                return (T &&) t;
            }
            using is_transparent = void;
        };

        struct plus
        {
            template<typename T, typename U>
            constexpr
            auto operator()(T &&t, U &&u) const ->
                decltype((T &&) t + (U &&) u)
            {
                return (T &&) t + (U &&) u;
            }
            using is_transparent = void;
        };

        struct minus
        {
            template<typename T, typename U>
            constexpr
            auto operator()(T &&t, U &&u) const ->
                decltype((T &&) t - (U &&) u)
            {
                return (T &&) t - (U &&) u;
            }
            using is_transparent = void;
        };

        struct multiplies
        {
            template<typename T, typename U>
            constexpr
            auto operator()(T &&t, U &&u) const ->
                decltype((T &&) t * (U &&) u)
            {
                return (T &&) t * (U &&) u;
            }
            using is_transparent = void;
        };

        struct bitwise_or
        {
            template<typename T, typename U>
            constexpr
            auto operator()(T &&t, U &&u) const ->
                decltype((T &&) t | (U &&) u)
            {
                return (T &&) t | (U &&) u;
            }
            using is_transparent = void;
        };

        template<typename T>
        struct convert_to
        {
            template<typename U>
            constexpr auto CPP_auto_fun(operator())(U &&u) (const)
            (
                return static_cast<T>((U &&) u)
            )
        };

        template<typename T>
        struct coerce
        {
            constexpr
            T & operator()(T & t) const
            {
                return t;
            }
            /// \overload
            constexpr
            T const & operator()(T const & t) const
            {
                return t;
            }
            /// \overload
            constexpr
            T operator()(T &&t) const
            {
                return (T &&) t;
            }
            T operator()(T const &&) const = delete;
        };

        template<typename T>
        struct coerce<T const>
          : coerce<T>
        {};

        template<typename T>
        struct coerce<T &>
          : coerce<T>
        {};

        template<typename T>
        struct coerce<T &&>
          : coerce<T>
        {};

        struct dereference_fn
        {
            template<typename I>
            constexpr auto CPP_auto_fun(operator())(I &i) (const)
            (
                return *i
            )
        };
        RANGES_INLINE_VARIABLE(dereference_fn, dereference)

        /// \addtogroup group-concepts
        /// @{
        CPP_def
        (
            template(typename Fun, typename... Args)
            (concept Invocable)(Fun, Args...),
                requires (Fun&& fn)
                (
                    invoke(static_cast<Fun &&>(fn), std::declval<Args>()...)
                )
        );

        CPP_def
        (
            template(typename Fun, typename... Args)
            (concept RegularInvocable)(Fun, Args...),
                Invocable<Fun, Args...>
                // Axiom: equality_preserving(invoke(f, args...))
        );

        CPP_def
        (
            template(typename Fun, typename... Args)
            (concept Predicate)(Fun, Args...),
                RegularInvocable<Fun, Args...> &&
                ConvertibleTo<invoke_result_t<Fun, Args...>, bool>
        );

        CPP_def
        (
            template(typename R, typename T, typename U)
            concept Relation,
                Predicate<R, T, T> &&
                Predicate<R, U, U> &&
                CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
                Predicate<
                    R,
                    common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>,
                    common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>> &&
                Predicate<R, T, U> &&
                Predicate<R, U, T>
        );

        template<typename FD>
        struct logical_negate_
        {
        private:
            CPP_assert(Same<FD, detail::decay_t<FD>> && MoveConstructible<FD>);
            FD pred_;
        public:
            CPP_member
            constexpr CPP_ctor(logical_negate_)()(
                noexcept(std::is_nothrow_default_constructible<FD>::value)
                requires DefaultConstructible<FD>)
            {}
            template<typename T>
            explicit constexpr CPP_ctor(logical_negate_)(T &&pred)(
                requires not defer::Same<detail::decay_t<T>, logical_negate_> &&
                    defer::Constructible<FD, T>)
              : pred_(static_cast<T &&>(pred))
            {}

            template<typename ...Args>
            constexpr /*c++14*/ auto operator()(Args &&...args) & ->
                CPP_ret(bool)(
                    requires Predicate<FD &, Args...>)
            {
                return !invoke(pred_, static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename ...Args>
            constexpr auto operator()(Args &&...args) const & ->
                CPP_ret(bool)(
                    requires Predicate<FD const &, Args...>)
            {
                return !invoke(pred_, static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename ...Args>
            constexpr /*c++14*/ auto operator()(Args &&...args) && ->
                CPP_ret(bool)(
                    requires Predicate<FD, Args...>)
            {
                return !invoke(static_cast<FD &&>(pred_), static_cast<Args &&>(args)...);
            }
        };

        template<typename Pred>
        using logical_negate = logical_negate_<detail::decay_t<Pred>>;

        struct not_fn_fn
        {
            template<typename Pred, typename FD = detail::decay_t<Pred>>
            constexpr auto operator()(Pred &&pred) const ->
                CPP_ret(logical_negate_<FD>)(
                    requires MoveConstructible<FD> && Constructible<FD, Pred>)
            {
                return logical_negate_<FD>{(Pred &&) pred};
            }
        };

        /// \ingroup group-utility
        /// \sa `not_fn_fn`
        RANGES_INLINE_VARIABLE(not_fn_fn, not_fn)

        template<typename Second, typename First>
        struct composed
        {
        private:
            RANGES_NO_UNIQUE_ADDRESS
            First first_;
            RANGES_NO_UNIQUE_ADDRESS
            Second second_;
            template<typename A, typename B, typename...Ts>
            static auto CPP_auto_fun(do_)(A &a, B &b, std::false_type, Ts &&...ts)
            (
                return invoke(b, invoke(a, (Ts &&) ts...))
            )
            template<typename A, typename B, typename...Ts>
            static auto CPP_auto_fun(do_)(A &a, B &b, std::true_type, Ts &&...ts)
            (
                return (invoke(a, (Ts &&) ts...),
                    invoke(b))
            )
        public:
            composed() = default;
            composed(Second second, First first)
              : first_(std::move(first))
              , second_(std::move(second))
            {}
            template<typename...Ts>
            auto CPP_auto_fun(operator())(Ts &&...ts)
            (
                return composed::do_(
                    first_, second_,
                    std::is_void<invoke_result_t<First &, Ts...>>{},
                    (Ts &&) ts...)
            )
            template<typename...Ts>
            auto CPP_auto_fun(operator())(Ts &&...ts) (const)
            (
                return composed::do_(
                    (First const &) first_, (Second const &) second_,
                    std::is_void<invoke_result_t<First const &, Ts...>>{},
                    (Ts &&) ts...)
            )
        };

        struct compose_fn
        {
            template<typename Second, typename First>
            composed<Second, First> operator()(Second second, First first) const
            {
                return {std::move(second), std::move(first)};
            }
        };

        /// \ingroup group-utility
        /// \sa `compose_fn`
        RANGES_INLINE_VARIABLE(compose_fn, compose)

        template<>
        struct overloaded<>
        {};

        template<typename First, typename...Rest>
        struct overloaded<First, Rest...>
        {
        private:
            RANGES_NO_UNIQUE_ADDRESS
            First first_;
            RANGES_NO_UNIQUE_ADDRESS
            overloaded<Rest...> second_;
        public:
            overloaded() = default;
            constexpr overloaded(First first, Rest... rest)
              : first_(detail::move(first))
              , second_{detail::move(rest)...}
            {}
            template<typename... Args>
            auto CPP_auto_fun(operator())(Args &&...args)
            (
                return invoke(first_, static_cast<Args &&>(args)...)
            )
            template<typename... Args>
            auto CPP_auto_fun(operator())(Args &&...args) (const)
            (
                return invoke((First const &) first_, static_cast<Args &&>(args)...)
            )
            template<typename... Args>
            auto CPP_auto_fun(operator())(Args &&...args)
            (
                return second_(static_cast<Args &&>(args)...)
            )
            template<typename... Args>
            auto CPP_auto_fun(operator())(Args &&...args) (const)
            (
                return ((overloaded<Rest...> const &) second_)(
                    static_cast<Args &&>(args)...)
            )
        };

        struct overload_fn
        {
            template<typename Fn>
            constexpr Fn operator()(Fn fn) const
            {
                return fn;
            }
            template<typename ...Fns>
            constexpr overloaded<Fns...> operator()(Fns... fns) const
            {
                return overloaded<Fns...>{detail::move(fns)...};
            }
        };

        /// \ingroup group-utility
        /// \sa `overload_fn`
        RANGES_INLINE_VARIABLE(overload_fn, overload)

        template<typename Fn>
        struct indirected
        {
        private:
            RANGES_NO_UNIQUE_ADDRESS
            Fn fn_;
        public:
            indirected() = default;
            indirected(Fn fn)
              : fn_(std::move(fn))
            {}
            // value_type (needs no impl)
            template<typename ...Its>
            [[noreturn]] auto operator()(copy_tag, Its...) const ->
                invoke_result_t<Fn &, iter_reference_t<Its>...>
            {
                RANGES_EXPECT(false);
            }

            // Reference
            template<typename ...Its>
            auto CPP_auto_fun(operator())(Its ...its)
            (
                return invoke(fn_, *its...)
            )
            template<typename ...Its>
            auto CPP_auto_fun(operator())(Its ...its) (const)
            (
                return invoke((Fn const &) fn_, *its...)
            )

            // Rvalue reference
            template<typename ...Its>
            auto CPP_auto_fun(operator())(move_tag, Its ...its)
            (
                return static_cast<
                    aux::move_t<invoke_result_t<Fn &, iter_reference_t<Its>...>>>(
                        aux::move(invoke(fn_, *its...)))
            )
            template<typename ...Its>
            auto CPP_auto_fun(operator())(move_tag, Its ...its) (const)
            (
                return static_cast<
                    aux::move_t<invoke_result_t<Fn const &, iter_reference_t<Its>...>>>(
                        aux::move(invoke((Fn const &) fn_, *its...)))
            )
        };

        struct indirect_fn
        {
            template<typename Fn>
            constexpr indirected<Fn> operator()(Fn fn) const
            {
                return indirected<Fn>{detail::move(fn)};
            }
        };

        /// \ingroup group-utility
        /// \sa `indirect_fn`
        RANGES_INLINE_VARIABLE(indirect_fn, indirect)

        template<typename Fn1, typename Fn2>
        struct transformed
        {
        private:
            RANGES_NO_UNIQUE_ADDRESS
            Fn1 first_;
            RANGES_NO_UNIQUE_ADDRESS
            Fn2 second_;
        public:
            transformed() = default;
            constexpr transformed(Fn1 fn1, Fn2 fn2)
              : first_(detail::move(fn1))
              , second_(detail::move(fn2))
            {}
            template<typename ...Args>
            auto CPP_auto_fun(operator())(Args &&... args)
            (
                return invoke(first_, invoke(second_, static_cast<Args &&>(args)...))
            )
            template<typename ...Args>
            auto CPP_auto_fun(operator())(Args &&... args) (const)
            (
                return invoke(
                    (Fn1 const &) first_,
                    invoke((Fn2 const &) second_, static_cast<Args &&>(args)...))
            )
        };

        struct on_fn
        {
            template<typename Fn1, typename Fn2>
            constexpr transformed<Fn1, Fn2> operator()(Fn1 fn1, Fn2 fn2) const
            {
                return transformed<Fn1, Fn2>{detail::move(fn1), detail::move(fn2)};
            }
        };

        /// \ingroup group-utility
        /// \sa `on_fn`
        RANGES_INLINE_VARIABLE(on_fn, on)

        /// \cond
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

            template<typename Pipe0, typename Pipe1>
            struct composed_pipe
            {
                Pipe0 pipe0_;
                Pipe1 pipe1_;
                template<typename Arg>
                auto CPP_auto_fun(operator())(Arg &&arg) (const)
                (
                    return static_cast<Arg &&>(arg) | pipe0_ | pipe1_
                )
            };
        }
        /// \endcond

        struct make_pipeable_fn
        {
            template<typename Fun>
            detail::pipeable_binder<Fun> operator()(Fun fun) const
            {
                return {std::move(fun)};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_pipeable_fn`
        RANGES_INLINE_VARIABLE(make_pipeable_fn, make_pipeable)

        template<typename T,
            typename U = meta::if_<
                std::is_lvalue_reference<T>,
                std::reference_wrapper<meta::_t<std::remove_reference<T>>>,
                T &&>>
        U bind_forward(meta::_t<std::remove_reference<T>> &t) noexcept
        {
            return static_cast<U>(t);
        }

        template<typename T>
        T &&bind_forward(meta::_t<std::remove_reference<T>> &&t) noexcept
        {
            // This is to catch way sketchy stuff like: forward<int const &>(42)
            static_assert(!std::is_lvalue_reference<T>::value, "You didn't just do that!");
            return static_cast<T &&>(t);
        }

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

        struct pipeable_access
        {
            template<typename Pipeable>
            struct impl_ : Pipeable
            {
                using Pipeable::pipe;
            };

            template<typename Pipeable>
            struct impl_<Pipeable &> : impl_<Pipeable>
            {};

            template<typename T>
            using impl = meta::if_c<is_pipeable<T>::value, impl_<T>>;
        };

        template<typename Derived>
        struct pipeable : pipeable_base
        {
        private:
            friend pipeable_access;
            // Default Pipe behavior just passes the argument to the pipe's function call
            // operator
            template<typename Arg, typename Pipe>
            static auto CPP_auto_fun(pipe)(Arg &&arg, Pipe pipe)
            (
                return pipe(static_cast<Arg &&>(arg))
            )
        };

        // Evaluate the pipe with an argument
        template<typename Arg, typename Pipe>
        auto operator|(Arg &&arg, Pipe pipe) ->
            CPP_ret(decltype(pipeable_access::impl<Pipe>::pipe(
                std::declval<Arg>(), std::declval<Pipe &>())))(
            requires not is_pipeable<Arg>::value && is_pipeable<Pipe>::value)
        {
            return pipeable_access::impl<Pipe>::pipe(static_cast<Arg &&>(arg), pipe);
        }

        // Compose two pipes
        template<typename Pipe0, typename Pipe1>
        auto operator|(Pipe0 pipe0, Pipe1 pipe1) ->
            CPP_ret(decltype(make_pipeable(
                std::declval<detail::composed_pipe<Pipe0, Pipe1>>())))(
            requires is_pipeable<Pipe0>::value && is_pipeable<Pipe1>::value)
        {
            return make_pipeable(detail::composed_pipe<Pipe0, Pipe1>{pipe0, pipe1});
        }

        template<typename T>
        struct bind_element
          : meta::if_<
                std::is_same<detail::decay_t<T>, T>,
                meta::id<T>,
                bind_element<detail::decay_t<T>>>
        {};

        template<typename T>
        struct bind_element<std::reference_wrapper<T>>
        {
            using type = T &;
        };

        template<typename T>
        struct bind_element<reference_wrapper<T>>
        {
            using type = typename reference_wrapper<T>::reference;
        };

        template<typename T>
        using bind_element_t = meta::_t<bind_element<T>>;

        struct ref_fn : pipeable<ref_fn>
        {
            template<typename T>
            auto operator()(T &t) const -> CPP_ret(reference_wrapper<T>)(
                requires not is_reference_wrapper_t<T>::value)
            {
                return {t};
            }
            /// \overload
            template<typename T>
            reference_wrapper<T> operator()(reference_wrapper<T> t) const
            {
                return t;
            }
            /// \overload
            template<typename T>
            reference_wrapper<T> operator()(std::reference_wrapper<T> t) const
            {
                return {t.get()};
            }
        };

        /// \ingroup group-utility
        /// \sa `ref_fn`
        RANGES_INLINE_VARIABLE(ref_fn, ref)

        template<typename T>
        using ref_t = decltype(ref(std::declval<T>()));

        struct unwrap_reference_fn : pipeable<unwrap_reference_fn>
        {
            template<typename T>
            T &&operator()(T &&t) const noexcept
            {
                return static_cast<T &&>(t);
            }
            /// \overload
            template<typename T>
            typename reference_wrapper<T>::reference
            operator()(reference_wrapper<T> t) const noexcept
            {
                return t.get();
            }
            /// \overload
            template<typename T>
            T &operator()(std::reference_wrapper<T> t) const noexcept
            {
                return t.get();
            }
            /// \overload
            template<typename T>
            T &operator()(ref_view<T> t) const noexcept
            {
                return t.base();
            }
        };

        /// \ingroup group-utility
        /// \sa `unwrap_reference_fn`
        RANGES_INLINE_VARIABLE(unwrap_reference_fn, unwrap_reference)

        template<typename T>
        using unwrap_reference_t = decltype(unwrap_reference(std::declval<T>()));

        /// \cond
        namespace detail
        {
            template<typename Bind>
            struct protect
            {
            private:
                Bind bind_;
            public:
                protect() = default;
                protect(Bind b)
                  : bind_(std::move(b))
                {}
                template<typename...Ts>
                auto CPP_auto_fun(operator())(Ts &&...ts)
                (
                    return bind_(static_cast<Ts &&>(ts)...)
                )
                /// \overload
                template<typename...Ts>
                auto CPP_auto_fun(operator())(Ts &&...ts) (const)
                (
                    return bind_(static_cast<Ts &&>(ts)...)
                )
            };
        }
        /// \endcond

        struct protect_fn
        {
            template<typename F>
            auto operator()(F &&f) const -> CPP_ret(detail::protect<uncvref_t<F>>)(
                requires std::is_bind_expression<uncvref_t<F>>::value)
            {
                return {static_cast<F &&>(f)};
            }
            /// \overload
            template<typename F>
            auto operator()(F &&f) const -> CPP_ret(F)(
                requires not std::is_bind_expression<uncvref_t<F>>::value)
            {
                return static_cast<F &&>(f);
            }
        };

        /// Protect a callable so that it can be safely used in a bind expression without
        /// accidentally becoming a "nested" bind.
        /// \ingroup group-utility
        /// \sa `protect_fn`
        RANGES_INLINE_VARIABLE(protect_fn, protect)

        // Accepts initializer_lists as either the first or second parameter, or both,
        // and forwards on to an implementation.
        template<typename ImplFn>
        struct with_braced_init_args
          : ImplFn
        {
        private:
            constexpr ImplFn const & base() const
            {
                return *this;
            }
        public:
            using ImplFn::operator();

            template<typename V0, typename...Args>
            constexpr auto operator()(std::initializer_list<V0> &&rng0, Args &&...args) const ->
                invoke_result_t<ImplFn const &, std::initializer_list<V0>, Args...>
            {
                return base()(std::move(rng0), static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename Rng0, typename V1, typename...Args>
            constexpr auto operator()(
                Rng0 &&rng0,
                std::initializer_list<V1> &&rng1,
                Args &&...args) const ->
                invoke_result_t<ImplFn const &, Rng0, std::initializer_list<V1>, Args...>
            {
                return base()(
                    static_cast<Rng0 &&>(rng0),
                    std::move(rng1),
                    static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename V0, typename V1, typename...Args>
            constexpr auto operator()(
                std::initializer_list<V0> &&rng0,
                std::initializer_list<V1> &&rng1,
                Args &&...args) const ->
                invoke_result_t<
                    ImplFn const &,
                    std::initializer_list<V0>,
                    std::initializer_list<V1>,
                    Args...>
            {
                return base()(std::move(rng0), std::move(rng1), static_cast<Args &&>(args)...);
            }
        };
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

#endif
