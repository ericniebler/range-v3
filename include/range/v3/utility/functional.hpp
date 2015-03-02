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
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

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
        };

        struct less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(WeaklyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
        };

        struct ordered_less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
        };

        struct ident
        {
            template<typename T>
            T && operator()(T && t) const noexcept
            {
                return (T &&) t;
            }
        };

        struct plus
        {
            template<typename T, typename U>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t + (U &&) u)
            {
                return (T &&) t + (U &&) u;
            }
        };

        struct minus
        {
            template<typename T, typename U>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t - (U &&) u)
            {
                return (T &&) t - (U &&) u;
            }
        };

        struct multiplies
        {
            template<typename T, typename U>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t * (U &&) u)
            {
                return (T &&) t * (U &&) u;
            }
        };

        struct bitwise_or
        {
            template<typename T, typename U>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t | (U &&) u)
            {
                return (T &&) t | (U &&) u;
            }
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

        /// \addtogroup group-utility
        struct make_invokable_fn
        {
        private:
            template<typename R, typename...Args>
            struct ptr_fn_
            {
            private:
                R (*pfn_)(Args...);
            public:
                ptr_fn_() = default;
                explicit ptr_fn_(R (*pfn)(Args...))
                  : pfn_(pfn)
                {}
                R operator()(Args...args) const
                {
                    return (*pfn_)(std::forward<Args>(args)...);
                }
            };
        public:
            template<typename R, typename ...Args>
            ptr_fn_<R, Args...> operator()(R (*p)(Args...)) const
            {
                return ptr_fn_<R, Args...>(p);
            }
            template<typename R, typename T>
            auto operator()(R T::* p) const -> decltype(std::mem_fn(p))
            {
                return std::mem_fn(p);
            }
            template<typename T, typename U = detail::decay_t<T>>
            auto operator()(T && t) const ->
                meta::if_c<!std::is_pointer<U>::value && !std::is_member_pointer<U>::value, T>
            {
                return std::forward<T>(t);
            }
        };

        /// \ingroup group-utility
        /// \sa `make_invokable_fn`
        namespace
        {
            constexpr auto&& invokable = static_const<make_invokable_fn>::value;
        }

        template<typename T>
        using invokable_t = decltype(invokable(std::declval<T>()));

        template<typename Pred>
        struct logical_negate
        {
        private:
            Pred pred_;
        public:
            logical_negate() = default;

            explicit constexpr logical_negate(Pred pred)
              : pred_((Pred &&) pred)
            {}

            template<typename T,
                CONCEPT_REQUIRES_(Predicate<Pred, T>())>
            bool operator()(T && t)
            {
                return !pred_((T &&) t);
            }
            /// \overload
            template<typename T,
                CONCEPT_REQUIRES_(Predicate<Pred const, T>())>
            constexpr bool operator()(T && t) const
            {
                return !pred_((T &&) t);
            }
            /// \overload
            template<typename T, typename U,
                CONCEPT_REQUIRES_(Predicate<Pred, T, U>())>
            bool operator()(T && t, U && u)
            {
                return !pred_((T &&) t, (U &&) u);
            }
            /// \overload
            template<typename T, typename U,
                CONCEPT_REQUIRES_(Predicate<Pred const, T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return !pred_((T &&) t, (U &&) u);
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
        namespace
        {
            constexpr auto&& not_ = static_const<not_fn>::value;
        }

        template<typename Second, typename First>
        struct composed
          : private box<Second, meta::size_t<0>>
          , private box<First, meta::size_t<1>>
        {
            composed() = default;
            composed(Second second, First first)
              : box<Second, meta::size_t<0>>{std::move(second)}
              , box<First, meta::size_t<1>>{std::move(first)}
            {}
            template<typename...Ts>
            auto operator()(Ts &&...ts)
            RANGES_DECLTYPE_AUTO_RETURN(
                ranges::get<0>(*this)(ranges::get<1>(*this)((Ts &&)ts)...)
            )
            template<typename...Ts>
            auto operator()(Ts &&...ts) const
            RANGES_DECLTYPE_AUTO_RETURN(
                ranges::get<0>(*this)(ranges::get<1>(*this)((Ts &&)ts)...)
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
        namespace
        {
            constexpr auto&& compose = static_const<compose_fn>::value;
        }

        template<>
        struct overloaded<>
        {};

        template<typename Fn>
        struct overloaded<Fn>
          : private invokable_t<Fn>
        {
            overloaded() = default;
            constexpr overloaded(Fn fn)
              : invokable_t<Fn>(invokable(std::move(fn)))
            {}
            using invokable_t<Fn>::operator();
        };

        template<typename First, typename...Rest>
        struct overloaded<First, Rest...>
          : private overloaded<First>
          , private overloaded<Rest...>
        {
            overloaded() = default;
            constexpr overloaded(First first, Rest... rest)
              : overloaded<First>{detail::move(first)}
              , overloaded<Rest...>{detail::move(rest)...}
            {}
            using overloaded<First>::operator();
            using overloaded<Rest...>::operator();
        };

        struct overload_fn
        {
            template<typename ...Fns>
            constexpr overloaded<Fns...> operator()(Fns... fns) const
            {
                return overloaded<Fns...>{detail::move(fns)...};
            }
        };

        /// \ingroup group-utility
        /// \sa `overload_fn`
        namespace
        {
            constexpr auto&& overload = static_const<overload_fn>::value;
        }

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
        namespace
        {
            constexpr auto&& make_pipeable = static_const<make_pipeable_fn>::value;
        }

        template<typename T,
            typename U = meta::if_<
                std::is_lvalue_reference<T>,
                std::reference_wrapper<meta::eval<std::remove_reference<T>>>,
                T &&>>
        U bind_forward(meta::eval<std::remove_reference<T>> & t) noexcept
        {
            return static_cast<U>(t);
        }

        template<typename T>
        T && bind_forward(meta::eval<std::remove_reference<T>> && t) noexcept
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
            static auto pipe(Arg && arg, Pipe && pipe)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                std::forward<Pipe>(pipe)(std::forward<Arg>(arg))
            )
        };

        // Evaluate the pipe with an argument
        template<typename Arg, typename Pipe,
            CONCEPT_REQUIRES_(!is_pipeable<Arg>() && is_pipeable<Pipe>())>
        auto operator|(Arg && arg, Pipe && pipe)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            pipeable_access::impl<Pipe>::pipe(std::forward<Arg>(arg), std::forward<Pipe>(pipe))
        )

        // Compose two pipes
        template<typename Pipe0, typename Pipe1,
            CONCEPT_REQUIRES_(is_pipeable<Pipe0>() && is_pipeable<Pipe1>())>
        auto operator|(Pipe0 && pipe0, Pipe1 && pipe1)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            make_pipeable(std::bind(
                bitwise_or{},
                std::bind(bitwise_or{}, std::placeholders::_1, bind_forward<Pipe0>(pipe0)),
                bind_forward<Pipe1>(pipe1)
            ))
        )

        template<typename T, bool RValue /* = false*/>
        struct reference_wrapper
        {
        private:
            T *t_;
        public:
            using type = T;
            using reference = meta::if_c<RValue, T &&, T &>;
            reference_wrapper() = default;
            reference_wrapper(reference t) noexcept
              : t_(std::addressof(t))
            {}
            reference get() const noexcept
            {
                RANGES_ASSERT(nullptr != t_);
                return static_cast<reference>(*t_);
            }
            operator reference() const noexcept
            {
                return get();
            }
            CONCEPT_REQUIRES(!RValue)
            operator std::reference_wrapper<T> () const noexcept
            {
                return {get()};
            }
            template<typename ...Args>
            auto operator()(Args &&...args) const ->
                decltype(std::declval<reference>()(std::declval<Args>()...))
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
        using is_reference_wrapper_t = meta::eval<is_reference_wrapper<T>>;

        template<typename T>
        struct referent_of
        {};

        template<typename T, bool RValue>
        struct referent_of<reference_wrapper<T, RValue>>
        {
            using type = T;
        };

        template<typename T>
        struct referent_of<std::reference_wrapper<T>>
        {
            using type = T;
        };

        template<typename T>
        struct referent_of<T &>
          : meta::if_<is_reference_wrapper<T>, referent_of<T>, meta::id<T>>
        {};

        template<typename T>
        struct referent_of<T &&>
          : meta::if_<is_reference_wrapper<T>, referent_of<T>, meta::id<T>>
        {};

        template<typename T>
        struct referent_of<T const>
          : referent_of<T>
        {};

        template<typename T>
        using referent_of_t = meta::eval<referent_of<T>>;

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
        using reference_of_t = meta::eval<reference_of<T>>;

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
        using bind_element_t = meta::eval<bind_element<T>>;

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
        namespace
        {
            constexpr auto&& ref = static_const<ref_fn>::value;
        }

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
        namespace
        {
            constexpr auto&& rref = static_const<rref_fn>::value;
        }

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
        namespace
        {
            constexpr auto&& unwrap_reference = static_const<unwrap_reference_fn>::value;
        }

        template<typename T>
        using unwrap_reference_t = decltype(unwrap_reference(std::declval<T>()));

        /// \ingroup group-utility
        template<typename T>
        using forward_ref_t =
            meta::if_<
                std::is_reference<T>,
                reference_wrapper<
                    meta::eval<std::remove_reference<T>>,
                    std::is_rvalue_reference<T>::value>,
                T &&>;

        /// Forward arguments, wrapping lvalue and rvalue refs in ranges::reference_wrapper.
        /// \ingroup group-utility
        template<typename T, typename U>
        forward_ref_t<T> forward_ref(U && t) noexcept
        {
            // This is to catch way sketchy stuff like: forward<int const &>(42)
            //static_assert(std::is_same<T &, U>::value, "You didn't just do that!");
            return static_cast<forward_ref_t<T>>((T &&) t);
        }

        /// \cond
        namespace detail
        {
            template<typename F>
            struct unwrap_args_fn
              : private box<F, meta::size_t<0>>
            {
                using expects_wrapped_references = void;
                unwrap_args_fn() = default;
                explicit unwrap_args_fn(F f)
                  : box<F, meta::size_t<0>>{std::move(f)}
                {}
                template<typename...Args>
                auto operator()(Args &&...args) ->
                    decltype(std::declval<F &>()(unwrap_reference(std::forward<Args>(args))...))
                {
                    return ranges::get<0>(*this)(unwrap_reference(std::forward<Args>(args))...);
                }
                template<typename...Args>
                auto operator()(Args &&...args) const ->
                    decltype(std::declval<F const &>()(unwrap_reference(std::forward<Args>(args))...))
                {
                    return ranges::get<0>(*this)(unwrap_reference(std::forward<Args>(args))...);
                }
            };

            template<typename T, typename Enable = void>
            struct expects_wrapped_references_
              : std::false_type
            {};

            template<typename T>
            struct expects_wrapped_references_<T, meta::void_<typename T::expects_wrapped_references>>
              : std::true_type
            {};
        }
        /// \endcond

        template<typename T>
        struct expects_wrapped_references
          : detail::expects_wrapped_references_<uncvref_t<T>>
        {};

        struct make_unwrap_args_fn
        {
            template<typename F, CONCEPT_REQUIRES_(!expects_wrapped_references<F>::value)>
            detail::unwrap_args_fn<F> operator()(F f) const
            {
                return detail::unwrap_args_fn<F>{std::move(f)};
            }
            template<typename F, CONCEPT_REQUIRES_(expects_wrapped_references<F>::value)>
            F operator()(F f) const
            {
                return std::move(f);
            }
        };

        /// \ingroup group-utility
        /// \sa `make_unwrap_args_fn`
        namespace
        {
            constexpr auto&& make_unwrap_args = static_const<make_unwrap_args_fn>::value;
        }

        template<typename F>
        using unwrap_args_t = decltype(make_unwrap_args(std::declval<F>()));

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
        namespace
        {
            constexpr auto&& protect = static_const<protect_fn>::value;
        }

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

#endif
