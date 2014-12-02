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
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/pipeable.hpp>

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
            T && operator()(T && t) const
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

        struct save_fn
        {
            template<typename T>
            T operator()(T && t) const
            {
                return (T &&) t;
            }
        };
        
        constexpr save_fn save{};

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
        constexpr not_fn not_ {};

        template<typename T>
        struct reference_wrapper
        {
        private:
            T *t_;
        public:
            using type = T;
            reference_wrapper() = default;
            reference_wrapper(T &t) noexcept
              : t_(std::addressof(t))
            {}
            T & get() const noexcept
            {
                RANGES_ASSERT(nullptr != t_);
                return *t_;
            }
            operator T &() const noexcept
            {
                return get();
            }
            template<typename ...Args>
            auto operator()(Args &&...args) const ->
                decltype((*t_)(std::forward<Args>(args)...))
            {
                return (*t_)(std::forward<Args>(args)...);
            }
        };

        template<typename T>
        struct is_reference_wrapper
          : std::false_type
        {};

        template<typename T>
        struct is_reference_wrapper<reference_wrapper<T>>
          : std::true_type
        {};

        template<typename T>
        struct is_reference_wrapper<std::reference_wrapper<T>>
          : std::true_type
        {};

        template<typename T>
        struct is_reference_wrapper<T &>
          : is_reference_wrapper<T>
        {};

        template<typename T>
        struct is_reference_wrapper<T const>
          : is_reference_wrapper<T>
        {};

        template<typename T>
        using is_reference_wrapper_t = meta::eval<is_reference_wrapper<T>>;

        template<typename T>
        struct referent_of
        {};

        template<typename T>
        struct referent_of<reference_wrapper<T>>
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
        struct referent_of<T const>
          : referent_of<T>
        {};

        template<typename T>
        using referent_of_t = meta::eval<referent_of<T>>;

        struct ref_fn : pipeable<ref_fn>
        {
            template<typename T, CONCEPT_REQUIRES_(!is_reference_wrapper_t<T>())>
            reference_wrapper<T> operator()(T & t) const
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
        constexpr ref_fn ref {};

        template<typename T>
        using ref_t = decltype(ref(std::declval<T>()));

        struct unwrap_reference_fn : pipeable<unwrap_reference_fn>
        {
            template<typename T, CONCEPT_REQUIRES_(!is_reference_wrapper<T>())>
            T & operator()(T & t) const noexcept
            {
                return t;
            }
            /// \overload
            template<typename T>
            T & operator()(reference_wrapper<T> t) const noexcept
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
        constexpr unwrap_reference_fn unwrap_reference {};

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
        constexpr protect_fn protect{};

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
        /// @{
    }
}

#endif
