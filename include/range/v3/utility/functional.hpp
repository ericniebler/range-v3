/// \file
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
#ifndef RANGES_V3_UTILITY_FUNCTIONAL_HPP
#define RANGES_V3_UTILITY_FUNCTIONAL_HPP

#include <memory> // std::addressof
#include <utility>
#include <functional> // std::reference_wrapper
#include <type_traits>
#include <initializer_list>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/compressed_pair.hpp>

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
            T && operator()(T && t) const noexcept
            {
                return (T &&) t;
            }
            using is_transparent = void;
        };

        struct plus
        {
            template<typename T, typename U>
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
            template<typename U,
                CONCEPT_REQUIRES_(std::is_constructible<T, U>::value)>
            constexpr auto operator()(U && u) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<T>((U &&) u)
            )
        };

        template<typename T>
        struct coerce
        {
            T & operator()(T & t) const
            {
                return t;
            }
            /// \overload
            T const & operator()(T const & t) const
            {
                return t;
            }
            /// \overload
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

        struct as_function_fn
        {
        private:
        #if __apple_build_version__
            // Work around a bug in earlier versions of libc++ that
            // shipped with Xcode
            template<typename MemFn>
            struct _mem_fn_wrap
            {
            private:
                mutable MemFn fn_;
            public:
                explicit _mem_fn_wrap(MemFn fn)
                  : fn_(std::move(fn))
                {}
                template<typename ...Ts>
                auto operator()(Ts &&... ts) const ->
                    decltype(fn_(std::forward<Ts>(ts)...))
                {
                    return fn_(std::forward<Ts>(ts)...);
                }
            };
            template<typename MemFn>
            _mem_fn_wrap<MemFn> _mem_fn_aux(MemFn fn) const
            {
                return _mem_fn_wrap<MemFn>(std::move(fn));
            }
            template<typename R, typename T>
            auto _mem_fn(R T::* p) const -> decltype(_mem_fn_aux(std::mem_fn(p)))
            {
                return _mem_fn_aux(std::mem_fn(p));
            }
        #else
            template<typename R, typename T>
            auto _mem_fn(R T::* p) const -> decltype(std::mem_fn(p))
            {
                return std::mem_fn(p);
            }
        #endif
            template<typename R, typename...Args>
            struct ptr_fn_
            {
            private:
                R (*pfn_)(Args...);
            public:
                ptr_fn_() = default;
                constexpr explicit ptr_fn_(R (*pfn)(Args...))
                  : pfn_(pfn)
                {}
                R operator()(Args...args) const
                {
                    return (*pfn_)(std::forward<Args>(args)...);
                }
            };
        public:
            template<typename R, typename ...Args>
            constexpr ptr_fn_<R, Args...> operator()(R (*p)(Args...)) const
            {
                return ptr_fn_<R, Args...>(p);
            }
            template<typename R, typename T>
            auto operator()(R T::* p) const -> decltype(_mem_fn(p))
            {
                return _mem_fn(p);
            }
            template<typename T, typename U = detail::decay_t<T>>
            constexpr auto operator()(T && t) const ->
                meta::if_c<!std::is_pointer<U>::value && !std::is_member_pointer<U>::value, T>
            {
                return detail::forward<T>(t);
            }
        };

        /// \ingroup group-utility
        /// \sa `make_invokable_fn`
        RANGES_INLINE_VARIABLE(as_function_fn, as_function)

        template<typename T>
        using function_type = decltype(as_function(std::declval<T>()));

        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct Callable
            {
                template<typename Fun, typename...Args>
                using result_t = Function::result_t<function_type<Fun>, Args...>;

                template<typename Fun, typename...Args>
                auto requires_(Fun&&, Args&&...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Function, function_type<Fun>, Args...>()
                    ));
            };

            struct RegularCallable
              : refines<Callable>
            {};

            struct CallablePredicate
              : refines<RegularCallable>
            {
                template<typename Fun, typename...Args>
                auto requires_(Fun&&, Args&&...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate, function_type<Fun>, Args...>()
                    ));
            };

            struct CallableRelation
              : refines<CallablePredicate>
            {
                template<typename Fun, typename T, typename U>
                auto requires_(Fun&&, T&&, U&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Relation, function_type<Fun>, T, U>()
                    ));
            };
        }

        template<typename Fun, typename...Args>
        using Callable = concepts::models<concepts::Callable, Fun, Args...>;

        template<typename Fun, typename...Args>
        using RegularCallable = concepts::models<concepts::RegularCallable, Fun, Args...>;

        template<typename Fun, typename...Args>
        using CallablePredicate = concepts::models<concepts::CallablePredicate, Fun, Args...>;

        template<typename Fun, typename T, typename U = T>
        using CallableRelation = concepts::models<concepts::CallableRelation, Fun, T, U>;
        /// @}

        template<typename Pred>
        struct logical_negate
        {
        private:
            using fn_t = meta::_t<std::decay<function_type<Pred>>>;
            fn_t pred_;
        public:
            logical_negate() = default;

            explicit constexpr logical_negate(Pred pred)
              : pred_(as_function((Pred &&) pred))
            {}

            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<fn_t&, Args...>())>
            bool operator()(Args &&...args)
            {
                return !pred_(((Args &&) args)...);
            }
            /// \overload
            template<typename ...Args,
                CONCEPT_REQUIRES_(Predicate<fn_t const&, Args...>())>
            constexpr bool operator()(Args &&...args) const
            {
                return !pred_(((Args &&) args)...);
            }
        };

        struct not_fn
        {
            template<typename Pred>
            constexpr logical_negate<Pred> operator()(Pred pred) const
            {
                return logical_negate<Pred>{(Pred &&) pred};
            }
        };

        /// \ingroup group-utility
        /// \sa `not_fn`
        RANGES_INLINE_VARIABLE(not_fn, not_)

        template<typename Second, typename First>
        struct composed
          : private compressed_pair<function_type<First>, function_type<Second>>
        {
        private:
            using composed::compressed_pair::first;
            using composed::compressed_pair::second;
            template<typename A, typename B, typename...Ts>
            static auto do_(A &a, B &b, std::false_type, Ts &&...ts)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                b(a((Ts &&) ts...))
            )
            template<typename A, typename B, typename...Ts>
            static auto do_(A &a, B &b, std::true_type, Ts &&...ts)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                (a((Ts &&) ts...),
                 b())
            )
        public:
            composed() = default;
            composed(Second second, First first)
              : composed::compressed_pair{
                    as_function(std::move(first)),
                    as_function(std::move(second))}
            {}
            template<typename...Ts,
                typename FirstResultT =
                    concepts::Function::result_t<function_type<First> &, Ts &&...>>
            auto operator()(Ts &&...ts)
            RANGES_DECLTYPE_NOEXCEPT(composed::do_(
                std::declval<function_type<First> &>(),
                std::declval<function_type<Second> &>(),
                std::is_void<FirstResultT>{},
                (Ts &&) ts...))
            {
                return composed::do_(
                    first(), second(),
                    std::is_void<FirstResultT>{},
                    (Ts &&) ts...);
            }
            template<typename...Ts,
                typename FirstResultT =
                    concepts::Function::result_t<function_type<First> const &, Ts &&...>>
            auto operator()(Ts &&...ts) const
            RANGES_DECLTYPE_NOEXCEPT(composed::do_(
                std::declval<function_type<First> const &>(),
                std::declval<function_type<Second> const &>(),
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
          : private compressed_pair<function_type<First>, overloaded<Rest...>>
        {
        private:
            using base_t = compressed_pair<function_type<First>, overloaded<Rest...>>;
            using base_t::first;
            using base_t::second;
        public:
            overloaded() = default;
            constexpr overloaded(First first, Rest... rest)
              : overloaded::compressed_pair{
                    as_function(detail::move(first)),
                    overloaded<Rest...>{detail::move(rest)...}}
            {}
            template<typename... Args>
            auto operator()(Args&&...args)
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<function_type<First> &>()(
                    detail::forward<Args>(args)...))
            {
                return first()(detail::forward<Args>(args)...);
            }
            template<typename... Args>
            auto operator()(Args&&...args) const
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<function_type<First> const &>()(
                    detail::forward<Args>(args)...))
            {
                return first()(detail::forward<Args>(args)...);
            }
            template<typename... Args>
            auto operator()(Args&&...args)
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<overloaded<Rest...> &>()(
                    detail::forward<Args>(args)...))
            {
                return second()(detail::forward<Args>(args)...);
            }
            template<typename... Args>
            auto operator()(Args&&...args) const
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<overloaded<Rest...> const &>()(
                    detail::forward<Args>(args)...))
            {
                return second()(detail::forward<Args>(args)...);
            }
        };

        struct overload_fn
        {
            template<typename Fn>
            constexpr function_type<Fn> operator()(Fn fn) const
            {
                return as_function(detail::move(fn));
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
          : private box<function_type<Fn>>
        {
        private:
            using BaseFn = function_type<Fn>;
            using box<BaseFn>::get;
        public:
            indirected() = default;
            indirected(Fn fn)
              : indirected::box(as_function(std::move(fn)))
            {}
            // value_type (needs no impl)
            template<typename ...Its>
            [[noreturn]] auto operator()(copy_tag, Its ...) const ->
                decltype(std::declval<BaseFn &>()(*std::declval<Its>()...))
            {
                RANGES_ENSURE(false);
            }

            // Reference
            template<typename ...Its>
            auto operator()(Its ...its)
            RANGES_DECLTYPE_NOEXCEPT(std::declval<BaseFn &>()(*its...))
            {
                return get()(*its...);
            }
            template<typename ...Its>
            auto operator()(Its ...its) const
            RANGES_DECLTYPE_NOEXCEPT(std::declval<BaseFn const &>()(*its...))
            {
                return get()(*its...);
            }

            // Rvalue reference
            template<typename ...Its>
            auto operator()(move_tag, Its ...its)
                noexcept(noexcept(aux::move(std::declval<BaseFn &>()(*its...)))) ->
                aux::move_t<decltype(std::declval<BaseFn &>()(*its...))>
            {
                return aux::move(get()(*its...));
            }
            template<typename ...Its>
            auto operator()(move_tag, Its ...its) const
                noexcept(noexcept(aux::move(std::declval<BaseFn const &>()(*its...)))) ->
                aux::move_t<decltype(std::declval<BaseFn const &>()(*its...))>
            {
                return aux::move(get()(*its...));
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
          : private compressed_pair<function_type<Fn1>, function_type<Fn2>>
        {
        private:
            using BaseFn1 = function_type<Fn1>;
            using BaseFn2 = function_type<Fn2>;
            using transformed::compressed_pair::first;
            using transformed::compressed_pair::second;

        public:
            transformed() = default;
            constexpr transformed(Fn1 fn1, Fn2 fn2)
              : transformed::compressed_pair{
                    as_function(detail::move(fn1)), as_function(detail::move(fn2))}
            {}
            template<typename ...Args>
            auto operator()(Args &&... args)
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<BaseFn1 &>()(std::declval<BaseFn2 &>()(std::forward<Args>(args))...))
            {
                return first()(second()(std::forward<Args>(args)...));
            }
            template<typename ...Args>
            auto operator()(Args &&... args) const
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<BaseFn1 const &>()(std::declval<BaseFn2 const &>()(std::forward<Args>(args))...))
            {
                return first()(second()(std::forward<Args>(args)...));
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
                auto operator()(Arg && arg) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::forward<Arg>(arg) | pipe0_ | pipe1_
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
        U bind_forward(meta::_t<std::remove_reference<T>> & t) noexcept
        {
            return static_cast<U>(t);
        }

        template<typename T>
        T && bind_forward(meta::_t<std::remove_reference<T>> && t) noexcept
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
                pipe(std::forward<Arg>(arg))
            )
        };

        // Evaluate the pipe with an argument
        template<typename Arg, typename Pipe,
            CONCEPT_REQUIRES_(!is_pipeable<Arg>() && is_pipeable<Pipe>())>
        auto operator|(Arg && arg, Pipe pipe)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            pipeable_access::impl<Pipe>::pipe(std::forward<Arg>(arg), pipe)
        )

        // Compose two pipes
        template<typename Pipe0, typename Pipe1,
            CONCEPT_REQUIRES_(is_pipeable<Pipe0>() && is_pipeable<Pipe1>())>
        auto operator|(Pipe0 pipe0, Pipe1 pipe1)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            make_pipeable(detail::composed_pipe<Pipe0, Pipe1>{pipe0, pipe1})
        )

        template<typename T, bool RValue /* = false*/>
        struct reference_wrapper
        {
        private:
            T *t_;
        public:
            using type = T;
            using reference = meta::if_c<RValue, T &&, T &>;
            constexpr reference_wrapper() = default;
            constexpr reference_wrapper(reference t) noexcept
              : t_(std::addressof(t))
            {}
            constexpr reference get() const noexcept
            {
                return static_cast<reference>(*t_);
            }
            constexpr operator reference() const noexcept
            {
                return get();
            }
            CONCEPT_REQUIRES(!RValue)
            operator std::reference_wrapper<T> () const noexcept
            {
                return {get()};
            }
            template<typename ...Args>
            constexpr auto operator()(Args &&...args) const
            RANGES_DECLTYPE_NOEXCEPT(
                std::declval<reference>()(std::declval<Args>()...))
            {
                return get()(std::forward<Args>(args)...);
            }
        };

        template<typename T>
        struct is_reference_wrapper
          : meta::if_<
                std::is_same<uncvref_t<T>, T>,
                std::false_type,
                is_reference_wrapper<uncvref_t<T>>>
        {};

        template<typename T, bool RValue>
        struct is_reference_wrapper<reference_wrapper<T, RValue>>
          : std::true_type
        {};

        template<typename T>
        struct is_reference_wrapper<std::reference_wrapper<T>>
          : std::true_type
        {};

        template<typename T>
        using is_reference_wrapper_t = meta::_t<is_reference_wrapper<T>>;

        template<typename T>
        struct reference_of
        {};

        template<typename T, bool RValue>
        struct reference_of<reference_wrapper<T, RValue>>
        {
            using type = meta::if_c<RValue, T &&, T &>;
        };

        template<typename T>
        struct reference_of<std::reference_wrapper<T>>
        {
            using type = T &;
        };

        template<typename T>
        struct reference_of<T &>
          : meta::if_<is_reference_wrapper<T>, reference_of<T>, meta::id<T &>>
        {};

        template<typename T>
        struct reference_of<T &&>
          : meta::if_<is_reference_wrapper<T>, reference_of<T>, meta::id<T &&>>
        {};

        template<typename T>
        struct reference_of<T const>
          : reference_of<T>
        {};

        template<typename T>
        using reference_of_t = meta::_t<reference_of<T>>;

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

        template<typename T, bool RValue>
        struct bind_element<reference_wrapper<T, RValue>>
        {
            using type = meta::if_c<RValue, T &&, T &>;
        };

        template<typename T>
        using bind_element_t = meta::_t<bind_element<T>>;

        struct ref_fn : pipeable<ref_fn>
        {
            template<typename T, CONCEPT_REQUIRES_(!is_reference_wrapper_t<T>())>
            reference_wrapper<T> operator()(T & t) const
            {
                return {t};
            }
            /// \overload
            template<typename T, bool RValue>
            reference_wrapper<T, RValue> operator()(reference_wrapper<T, RValue> t) const
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

        struct rref_fn : pipeable<rref_fn>
        {
            template<typename T,
                CONCEPT_REQUIRES_(!is_reference_wrapper_t<T>() &&
                    !std::is_lvalue_reference<T>::value)>
            reference_wrapper<T, true> operator()(T && t) const
            {
                return {std::move(t)};
            }
            /// \overload
            template<typename T>
            reference_wrapper<T, true> operator()(reference_wrapper<T, true> t) const
            {
                return t;
            }
        };

        /// \ingroup group-utility
        /// \sa `rref_fn`
        RANGES_INLINE_VARIABLE(rref_fn, rref)

        template<typename T>
        using rref_t = decltype(rref(std::declval<T>()));

        struct unwrap_reference_fn : pipeable<unwrap_reference_fn>
        {
            template<typename T, CONCEPT_REQUIRES_(!is_reference_wrapper<T>())>
            T && operator()(T && t) const noexcept
            {
                return std::forward<T>(t);
            }
            /// \overload
            template<typename T, bool RValue>
            meta::if_c<RValue, T &&, T &> operator()(reference_wrapper<T, RValue> t) const noexcept
            {
                return t.get();
            }
            /// \overload
            template<typename T>
            T & operator()(std::reference_wrapper<T> t) const noexcept
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
                    bind_(std::forward<Ts>(ts)...)
                )
                /// \overload
                template<typename...Ts>
                auto operator()(Ts &&...ts) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    bind_(std::forward<Ts>(ts)...)
                )
            };
        }
        /// \endcond

        struct protect_fn
        {
            template<typename F, CONCEPT_REQUIRES_(std::is_bind_expression<uncvref_t<F>>())>
            detail::protect<uncvref_t<F>> operator()(F && f) const
            {
                return {std::forward<F>(f)};
            }
            /// \overload
            template<typename F, CONCEPT_REQUIRES_(!std::is_bind_expression<uncvref_t<F>>())>
            F operator()(F && f) const
            {
                return std::forward<F>(f);
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
            ImplFn const & base() const
            {
                return *this;
            }
        public:
            using ImplFn::operator();

            template<typename V0, typename...Args>
            auto operator()(std::initializer_list<V0> &&rng0, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::move(rng0), std::declval<Args>()...))
            {
                return base()(std::move(rng0), std::forward<Args>(args)...);
            }
            /// \overload
            template<typename Rng0, typename V1, typename...Args>
            auto operator()(Rng0 && rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::declval<Rng0>(), std::move(rng1), std::declval<Args>()...))
            {
                return base()(std::forward<Rng0>(rng0), std::move(rng1), std::forward<Args>(args)...);
            }
            /// \overload
            template<typename V0, typename V1, typename...Args>
            auto operator()(std::initializer_list<V0> rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::move(rng0), std::move(rng1), std::declval<Args>()...))
            {
                return base()(std::move(rng0), std::move(rng1), std::forward<Args>(args)...);
            }
        };
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

#endif
