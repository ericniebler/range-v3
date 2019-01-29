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
            template<typename T, typename U,
                CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t == (U &&) u;
            }
            using is_transparent = void;
        };

        struct not_equal_to
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t != (U &&) u;
            }
            using is_transparent = void;
        };

        struct less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(WeaklyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
            using is_transparent = void;
        };

        struct ordered_less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
            using is_transparent = void;
        };

        struct ident
        {
            template<typename T>
            constexpr
            T && operator()(T && t) const noexcept
            {
                return (T &&) t;
            }
            using is_transparent = void;
        };

        struct plus
        {
            template<typename T, typename U>
            constexpr
            auto operator()(T && t, U && u) const ->
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
            auto operator()(T && t, U && u) const ->
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
            auto operator()(T && t, U && u) const ->
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
            auto operator()(T && t, U && u) const ->
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
            constexpr auto operator()(U && u) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<T>((U &&) u)
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
            T operator()(T && t) const
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
            constexpr auto operator()(I &i) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                *i
            )
        };
        RANGES_INLINE_VARIABLE(dereference_fn, dereference)

        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct Invocable
            {
                template<typename Fun, typename... Args>
                using result_t = invoke_result_t<Fun, Args...>;

                template<typename Fun, typename... Args>
                auto requires_() ->
                    meta::void_<invoke_result_t<Fun, Args...>>;
            };

            struct RegularInvocable
              : refines<Invocable>
            {
                // Axiom: equality_preserving(invoke(f, args...))
            };

            struct Predicate
              : refines<RegularInvocable>
            {
                template<typename Fun, typename... Args>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConvertibleTo, Invocable::result_t<Fun, Args...>, bool>()
                    ));
            };

            struct Relation
            {
                template<typename Fun, typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate, Fun, T, T>()
                    ));

                template<typename Fun, typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_same<T, U>{}),
                        concepts::model_of<Predicate, Fun, T, U>()
                    ));

                template<typename Fun, typename T, typename U,
                    typename C =
                        common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(std::is_same<T, U>{}),
                        concepts::model_of<Relation, Fun, T, T>(),
                        concepts::model_of<Relation, Fun, U, U>(),
                        concepts::model_of<
                            CommonReference, detail::as_cref_t<T>, detail::as_cref_t<U>>(),
                        concepts::model_of<Relation, Fun, C, C>(),
                        concepts::model_of<Predicate, Fun, T, U>(),
                        concepts::model_of<Predicate, Fun, U, T>()
                    ));
            };
        }

        template<typename Fun, typename...Args>
        using Invocable = concepts::models<concepts::Invocable, Fun, Args...>;

        template<typename Fun, typename...Args>
        using RegularInvocable = concepts::models<concepts::RegularInvocable, Fun, Args...>;

        template<typename Fun, typename...Args>
        using Predicate = concepts::models<concepts::Predicate, Fun, Args...>;

        template<typename Fun, typename T, typename U = T>
        using Relation = concepts::models<concepts::Relation, Fun, T, U>;
        /// @}

        template<typename FD>
        struct logical_negate_
        {
        private:
            CONCEPT_ASSERT(Same<FD, detail::decay_t<FD>>() && MoveConstructible<FD>());
            FD pred_;
        public:
            CONCEPT_REQUIRES(DefaultConstructible<FD>())
            constexpr logical_negate_()
                noexcept(std::is_nothrow_default_constructible<FD>::value)
            {}
            template<typename T,
                typename U = meta::if_c<!Same<detail::decay_t<T>, logical_negate_>(), T>,
                CONCEPT_REQUIRES_(Constructible<FD, U>())>
            explicit constexpr logical_negate_(T && pred)
              : pred_(static_cast<T &&>(pred))
            {}

// HACKHACKHACK GCC 4.8 is extremely confused about && and const&& qualifiers.
// Luckily they are rare - we'll simply break them.
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5 && __GNUC_MINOR__ < 9
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<FD &, Args...>())>
            RANGES_CXX14_CONSTEXPR auto operator()(Args &&...args)
            RANGES_DECLTYPE_NOEXCEPT(
                !invoke(std::declval<FD &>(), static_cast<Args &&>(args)...))
            {
                return !invoke(pred_, static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<FD const &, Args...>())>
            constexpr auto operator()(Args &&...args) const
            RANGES_DECLTYPE_NOEXCEPT(
                !invoke(std::declval<FD const &>(), static_cast<Args &&>(args)...))
            {
                return !invoke(pred_, static_cast<Args &&>(args)...);
            }
#else // ^^^ GCC <= 4.8 / GCC > 4.8 vvvv
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<FD &, Args...>())>
            RANGES_CXX14_CONSTEXPR auto operator()(Args &&...args) &
            RANGES_DECLTYPE_NOEXCEPT(
                !invoke(std::declval<FD &>(), static_cast<Args &&>(args)...))
            {
                return !invoke(pred_, static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<FD const &, Args...>())>
            constexpr auto operator()(Args &&...args) const &
            RANGES_DECLTYPE_NOEXCEPT(
                !invoke(std::declval<FD const &>(), static_cast<Args &&>(args)...))
            {
                return !invoke(pred_, static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<FD, Args...>())>
            RANGES_CXX14_CONSTEXPR auto operator()(Args &&...args) &&
            RANGES_DECLTYPE_NOEXCEPT(
                !invoke(std::declval<FD>(), static_cast<Args &&>(args)...))
            {
                return !invoke(static_cast<FD &&>(pred_), static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<FD const, Args...>())>
            RANGES_CXX14_CONSTEXPR auto operator()(Args &&...args) const &&
            RANGES_DECLTYPE_NOEXCEPT(
                !invoke(std::declval<FD const>(), static_cast<Args &&>(args)...))
            {
                return !invoke(static_cast<FD const &&>(pred_), static_cast<Args &&>(args)...);
            }
#endif // GCC
        };

        struct not_fn_fn
        {
            template<typename Pred, typename FD = detail::decay_t<Pred>,
                CONCEPT_REQUIRES_(MoveConstructible<FD>() && Constructible<FD, Pred>())>
            constexpr logical_negate_<FD> operator()(Pred && pred) const
            {
                return logical_negate_<FD>{(Pred &&) pred};
            }
        };

        /// \ingroup group-utility
        /// \sa `not_fn_fn`
        RANGES_INLINE_VARIABLE(not_fn_fn, not_fn)

        /// \cond
        inline namespace
        {
            RANGES_DEPRECATED("\"not_\" now uses the C++17 name \"not_fn\".")
            constexpr const auto& not_ = not_fn;
        }
        /// \endcond

        template<typename Second, typename First>
        struct composed
          : private compressed_pair<First, Second>
        {
        private:
            using composed::compressed_pair::first;
            using composed::compressed_pair::second;
            template<typename A, typename B, typename...Ts>
            static auto do_(A &a, B &b, std::false_type, Ts &&...ts)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                invoke(b, invoke(a, (Ts &&) ts...))
            )
            template<typename A, typename B, typename...Ts>
            static auto do_(A &a, B &b, std::true_type, Ts &&...ts)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                (invoke(a, (Ts &&) ts...),
                 invoke(b))
            )
        public:
            composed() = default;
            composed(Second second, First first)
              : composed::compressed_pair{std::move(first), std::move(second)}
            {}
            template<typename...Ts,
                typename FirstResultT = invoke_result_t<First&, Ts...>>
            auto operator()(Ts &&...ts)
            RANGES_DECLTYPE_NOEXCEPT(composed::do_(
                std::declval<First &>(),
                std::declval<Second &>(),
                std::is_void<FirstResultT>{},
                (Ts &&) ts...))
            {
                return composed::do_(
                    first(), second(),
                    std::is_void<FirstResultT>{},
                    (Ts &&) ts...);
            }
            template<typename...Ts,
                typename FirstResultT = invoke_result_t<First const &, Ts...>>
            auto operator()(Ts &&...ts) const
            RANGES_DECLTYPE_NOEXCEPT(composed::do_(
                std::declval<First const &>(),
                std::declval<Second const &>(),
                std::is_void<FirstResultT>{},
                (Ts &&) ts...))
            {
                return composed::do_(
                    first(), second(),
                    std::is_void<FirstResultT>{},
                    (Ts &&) ts...);
            }
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
          : private compressed_pair<First, overloaded<Rest...>>
        {
        private:
            using base_t = compressed_pair<First, overloaded<Rest...>>;
            using base_t::first;
            using base_t::second;
        public:
            overloaded() = default;
            constexpr overloaded(First first, Rest... rest)
              : overloaded::compressed_pair{
                    detail::move(first),
                    overloaded<Rest...>{detail::move(rest)...}}
            {}
            template<typename... Args>
            auto operator()(Args&&...args)
            RANGES_DECLTYPE_NOEXCEPT(
                invoke(std::declval<First &>(),
                    static_cast<Args&&>(args)...))
            {
                return invoke(first(), static_cast<Args&&>(args)...);
            }
            template<typename... Args>
            auto operator()(Args&&...args) const
            RANGES_DECLTYPE_NOEXCEPT(
                invoke(std::declval<First const &>(),
                    static_cast<Args&&>(args)...))
            {
                return invoke(first(), static_cast<Args&&>(args)...);
            }
            template<typename... Args>
            auto operator()(Args&&...args)
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<overloaded<Rest...> &>()(
                    static_cast<Args&&>(args)...))
            {
                return second()(static_cast<Args&&>(args)...);
            }
            template<typename... Args>
            auto operator()(Args&&...args) const
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<overloaded<Rest...> const &>()(
                    static_cast<Args&&>(args)...))
            {
                return second()(static_cast<Args&&>(args)...);
            }
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
          : private box<Fn, indirected<Fn>>
        {
        private:
            using box<Fn, indirected<Fn>>::get;
        public:
            indirected() = default;
            indirected(Fn fn)
              : indirected::box(std::move(fn))
            {}
            // value_type (needs no impl)
            template<typename ...Its>
            [[noreturn]] auto operator()(copy_tag, Its...) const ->
                invoke_result_t<Fn &, reference_t<Its>...>
            {
                RANGES_EXPECT(false);
            }

            // Reference
            template<typename ...Its>
            auto operator()(Its ...its)
            RANGES_DECLTYPE_NOEXCEPT(invoke(std::declval<Fn &>(), *its...))
            {
                return invoke(get(), *its...);
            }
            template<typename ...Its>
            auto operator()(Its ...its) const
            RANGES_DECLTYPE_NOEXCEPT(invoke(std::declval<Fn const &>(), *its...))
            {
                return invoke(get(), *its...);
            }

            // Rvalue reference
            template<typename ...Its>
            auto operator()(move_tag, Its ...its)
                noexcept(noexcept(aux::move(invoke(std::declval<Fn &>(), *its...)))) ->
                aux::move_t<decltype(invoke(std::declval<Fn &>(), *its...))>
            {
                return aux::move(invoke(get(), *its...));
            }
            template<typename ...Its>
            auto operator()(move_tag, Its ...its) const
                noexcept(noexcept(aux::move(invoke(std::declval<Fn const &>(), *its...)))) ->
                aux::move_t<decltype(invoke(std::declval<Fn const &>(), *its...))>
            {
                return aux::move(invoke(get(), *its...));
            }
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
          : private compressed_pair<Fn1, Fn2>
        {
        private:
            using transformed::compressed_pair::first;
            using transformed::compressed_pair::second;

        public:
            transformed() = default;
            constexpr transformed(Fn1 fn1, Fn2 fn2)
              : transformed::compressed_pair{detail::move(fn1), detail::move(fn2)}
            {}
            template<typename ...Args>
            auto operator()(Args &&... args)
            RANGES_DECLTYPE_NOEXCEPT(
                invoke(std::declval<Fn1 &>(), invoke(std::declval<Fn2 &>(), static_cast<Args&&>(args))...))
            {
                return invoke(first(), invoke(second(), static_cast<Args&&>(args)...));
            }
            template<typename ...Args>
            auto operator()(Args &&... args) const
            RANGES_DECLTYPE_NOEXCEPT(
                invoke(std::declval<Fn1 const &>(), invoke(std::declval<Fn2 const &>(), static_cast<Args&&>(args))...))
            {
                return invoke(first(), invoke(second(), static_cast<Args&&>(args)...));
            }
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
            struct RANGES_EMPTY_BASES pipeable_binder
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
                auto operator()(Arg && arg) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    static_cast<Arg&&>(arg) | pipe0_ | pipe1_
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
        T && bind_forward(meta::_t<std::remove_reference<T>> &&t) noexcept
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
            struct impl : Pipeable
            {
                using Pipeable::pipe;
            };

            template<typename Pipeable>
            struct impl<Pipeable &> : impl<Pipeable>
            {};
        };

        template<typename Derived>
        struct pipeable : pipeable_base
        {
        private:
            friend pipeable_access;
            // Default Pipe behavior just passes the argument to the pipe's function call
            // operator
            template<typename Arg, typename Pipe>
            static auto pipe(Arg && arg, Pipe pipe)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pipe(static_cast<Arg&&>(arg))
            )
        };

        // Evaluate the pipe with an argument
        template<typename Arg, typename Pipe,
            CONCEPT_REQUIRES_(!is_pipeable<Arg>() && is_pipeable<Pipe>())>
        auto operator|(Arg && arg, Pipe pipe)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            pipeable_access::impl<Pipe>::pipe(static_cast<Arg&&>(arg), pipe)
        )

        // Compose two pipes
        template<typename Pipe0, typename Pipe1,
            CONCEPT_REQUIRES_(is_pipeable<Pipe0>() && is_pipeable<Pipe1>())>
        auto operator|(Pipe0 pipe0, Pipe1 pipe1)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            make_pipeable(detail::composed_pipe<Pipe0, Pipe1>{pipe0, pipe1})
        )

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
            template<typename T, CONCEPT_REQUIRES_(!is_reference_wrapper_t<T>())>
            reference_wrapper<T> operator()(T &t) const
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
            template<typename T, CONCEPT_REQUIRES_(!is_reference_wrapper<T>())>
            T &&operator()(T &&t) const noexcept
            {
                return static_cast<T&&>(t);
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
                auto operator()(Ts &&...ts)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    bind_(static_cast<Ts&&>(ts)...)
                )
                /// \overload
                template<typename...Ts>
                auto operator()(Ts &&...ts) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    bind_(static_cast<Ts&&>(ts)...)
                )
            };
        }
        /// \endcond

        struct protect_fn
        {
            template<typename F, CONCEPT_REQUIRES_(std::is_bind_expression<uncvref_t<F>>())>
            detail::protect<uncvref_t<F>> operator()(F && f) const
            {
                return {static_cast<F&&>(f)};
            }
            /// \overload
            template<typename F, CONCEPT_REQUIRES_(!std::is_bind_expression<uncvref_t<F>>())>
            F operator()(F && f) const
            {
                return static_cast<F&&>(f);
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
            constexpr
            auto operator()(std::initializer_list<V0> &&rng0, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::move(rng0), std::declval<Args>()...))
            {
                return base()(std::move(rng0), static_cast<Args&&>(args)...);
            }
            /// \overload
            template<typename Rng0, typename V1, typename...Args>
            constexpr
            auto operator()(Rng0 && rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::declval<Rng0>(), std::move(rng1), std::declval<Args>()...))
            {
                return base()(static_cast<Rng0&&>(rng0), std::move(rng1), static_cast<Args&&>(args)...);
            }
            /// \overload
            template<typename V0, typename V1, typename...Args>
            constexpr
            auto operator()(std::initializer_list<V0> rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::move(rng0), std::move(rng1), std::declval<Args>()...))
            {
                return base()(std::move(rng0), std::move(rng1), static_cast<Args&&>(args)...);
            }
        };
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

#endif
