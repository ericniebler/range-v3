/// \file
// Concepts library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef CONCEPTS_SWAP_HPP
#define CONCEPTS_SWAP_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>

// Note: constexpr implies inline, to retain the same visibility
// C++14 constexpr functions are inline in C++11
#if (defined(__cpp_constexpr) && __cpp_constexpr >= 201304L) ||\
    (!defined(__cpp_constexpr) && __cplusplus >= 201402L)
#define CONCEPTS_CXX14_CONSTEXPR constexpr
#else
#define CONCEPTS_CXX14_CONSTEXPR inline
#endif

#ifndef CONCEPTS_CXX_INLINE_VARIABLES
#ifdef __cpp_inline_variables // TODO: fix this if SD-6 picks another name
#define CONCEPTS_CXX_INLINE_VARIABLES __cpp_inline_variables
// TODO: remove once clang defines __cpp_inline_variables (or equivalent)
#elif defined(__clang__) && \
    (__clang_major__ > 3 || __clang_major__ == 3 && __clang_minor__ == 9) && \
    __cplusplus > 201402L
#define CONCEPTS_CXX_INLINE_VARIABLES 201606L
#else
#define CONCEPTS_CXX_INLINE_VARIABLES __cplusplus
#endif  // __cpp_inline_variables
#endif  // CONCEPTS_CXX_INLINE_VARIABLES

#if defined(_MSC_VER) && !defined(__clang__)
#define CONCEPTS_WORKAROUND_MSVC_620035 // Error when definition-context name binding finds only deleted function
#endif

#if CONCEPTS_CXX_INLINE_VARIABLES < 201606L
#define CONCEPTS_INLINE_VARIABLE(type, name)                                    \
    inline namespace                                                            \
    {                                                                           \
        constexpr auto &name = ::concepts::detail::static_const<type>::value;   \
    }                                                                           \
    /**/
#else  // CONCEPTS_CXX_INLINE_VARIABLES >= 201606L
#define CONCEPTS_INLINE_VARIABLE(type, name)                                    \
    inline constexpr type name{};                                               \
    /**/
#endif // CONCEPTS_CXX_INLINE_VARIABLES

#if CONCEPTS_CXX_INLINE_VARIABLES < 201606L
#define CONCEPTS_DEFINE_CPO(type, name)                                         \
    inline namespace                                                            \
    {                                                                           \
        constexpr auto &name = ::concepts::detail::static_const<type>::value;   \
    }                                                                           \
    /**/
#else  // CONCEPTS_CXX_INLINE_VARIABLES >= 201606L
#define CONCEPTS_DEFINE_CPO(type, name)                                         \
    inline namespace _                                                          \
    {                                                                           \
        inline constexpr type name{};                                           \
    }                                                                           \
    /**/
#endif // CONCEPTS_CXX_INLINE_VARIABLES

#if defined(_MSC_VER) && !defined(__clang__)
#define CONCEPTS_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
#else // ^^^ defined(_MSC_VER) ^^^ / vvv !defined(_MSC_VER) vvv
#if defined(__GNUC__) || defined(__clang__)
#define CONCEPTS_PRAGMA(X) _Pragma(#X)
#define CONCEPTS_DIAGNOSTIC_IGNORE_PRAGMAS \
    CONCEPTS_PRAGMA(GCC diagnostic ignored "-Wpragmas")
#define CONCEPTS_DIAGNOSTIC_IGNORE(X) \
    CONCEPTS_DIAGNOSTIC_IGNORE_PRAGMAS \
    CONCEPTS_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas") \
    CONCEPTS_PRAGMA(GCC diagnostic ignored X)
#define CONCEPTS_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME \
    CONCEPTS_DIAGNOSTIC_IGNORE("-Wunknown-warning-option") \
    CONCEPTS_DIAGNOSTIC_IGNORE("-Winit-list-lifetime")
#else
#define CONCEPTS_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
#endif
#endif // MSVC/Generic configuration switch

namespace concepts
{
    /// \cond
    namespace detail
    {
        template<typename T>
        struct is_movable_
            : meta::and_<
                std::is_object<T>,
                std::is_move_constructible<T>,
                std::is_move_assignable<T>>
        {};

        template<typename T>
        struct static_const
        {
            static constexpr T const value {};
        };
        template<typename T>
        constexpr T const static_const<T>::value;
    }
    /// \endcond

    template<typename T>
    struct is_swappable;

    template<typename T>
    struct is_nothrow_swappable;

    template<typename T, typename U>
    struct is_swappable_with;

    template<typename T, typename U>
    struct is_nothrow_swappable_with;

    template<typename T, typename U = T>
    CONCEPTS_CXX14_CONSTEXPR
    meta::if_c<
        std::is_move_constructible<T>::value &&
        std::is_assignable<T &, U>::value, T>
    exchange(T &t, U &&u)
        noexcept(
            std::is_nothrow_move_constructible<T>::value &&
            std::is_nothrow_assignable<T &, U>::value)
    {
        T tmp((T &&) t);
        t = (U &&) u;
        CONCEPTS_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
        return tmp;
    }

    /// \cond
    namespace adl_swap_detail
    {
        // Intentionally create an ambiguity with std::swap, which is
        // (possibly) unconstrained.
        template<typename T>
        void swap(T &, T &) = delete;

        template<typename T, std::size_t N>
        void swap(T (&)[N], T (&)[N]) = delete;

#ifdef CONCEPTS_WORKAROUND_MSVC_620035
        void swap();
#endif

        template<typename T, typename U,
            typename = decltype(swap(std::declval<T>(), std::declval<U>()))>
        std::true_type try_adl_swap_(int);

        template<typename T, typename U>
        std::false_type try_adl_swap_(long);

        template<typename T, typename U = T>
        struct is_adl_swappable_
            : meta::id_t<decltype(adl_swap_detail::try_adl_swap_<T, U>(42))>
        {};

        struct swap_fn
        {
            // Dispatch to user-defined swap found via ADL:
            template<typename T, typename U>
            CONCEPTS_CXX14_CONSTEXPR
            meta::if_c<is_adl_swappable_<T, U>::value>
            operator()(T &&t, U &&u) const
            noexcept(noexcept(swap((T &&) t, (U &&) u)))
            {
                swap((T &&) t, (U &&) u);
            }

            // For intrinsically swappable (i.e., movable) types for which
            // a swap overload cannot be found via ADL, swap by moving.
            template<typename T>
            CONCEPTS_CXX14_CONSTEXPR
            meta::if_c<
                !is_adl_swappable_<T &>::value &&
                detail::is_movable_<T>::value>
            operator()(T &a, T &b) const
            noexcept(noexcept(b = concepts::exchange(a, (T &&) b)))
            {
                b = concepts::exchange(a, (T &&) b);
            }

            // For arrays of intrinsically swappable (i.e., movable) types
            // for which a swap overload cannot be found via ADL, swap array
            // elements by moving.
            template<typename T, typename U, std::size_t N>
            CONCEPTS_CXX14_CONSTEXPR
            meta::if_c<
                !is_adl_swappable_<T (&)[N], U (&)[N]>::value &&
                is_swappable_with<T &, U &>::value>
            operator()(T (&t)[N], U (&u)[N]) const
                noexcept(is_nothrow_swappable_with<T &, U &>::value)
            {
                for(std::size_t i = 0; i < N; ++i)
                    (*this)(t[i], u[i]);
            }

            // For rvalue pairs and tuples of swappable types, swap the
            // members. This permits code like:
            //   ranges::swap(std::tie(a,b,c), std::tie(d,e,f));
            template<typename F0, typename S0, typename F1, typename S1>
            CONCEPTS_CXX14_CONSTEXPR
            meta::if_c<is_swappable_with<F0, F1>::value && is_swappable_with<S0, S1>::value>
            operator()(std::pair<F0, S0> &&left, std::pair<F1, S1> &&right) const
                noexcept(
                    is_nothrow_swappable_with<F0, F1>::value &&
                    is_nothrow_swappable_with<S0, S1>::value)
            {
                swap_fn()(static_cast<std::pair<F0, S0> &&>(left).first,
                            static_cast<std::pair<F1, S1> &&>(right).first);
                swap_fn()(static_cast<std::pair<F0, S0> &&>(left).second,
                            static_cast<std::pair<F1, S1> &&>(right).second);
            }

            template<typename ...Ts, typename ...Us>
            CONCEPTS_CXX14_CONSTEXPR
            meta::if_c<meta::and_c<is_swappable_with<Ts, Us>::value...>::value>
            operator()(std::tuple<Ts...> &&left, std::tuple<Us...> &&right) const
                noexcept(meta::and_c<is_nothrow_swappable_with<Ts, Us>::value...>::value)
            {
                swap_fn::impl(
                    static_cast<std::tuple<Ts...> &&>(left),
                    static_cast<std::tuple<Us...> &&>(right),
                    meta::make_index_sequence<sizeof...(Ts)>{});
            }

        private:
            template<typename... Ts>
            static constexpr int ignore_unused(Ts &&...)
            {
                return 0;
            }
            template<typename T, typename U, std::size_t ...Is>
            CONCEPTS_CXX14_CONSTEXPR
            static void impl(T &&left, U &&right, meta::index_sequence<Is...>)
            {
                (void) swap_fn::ignore_unused(
                    (swap_fn()(std::get<Is>(static_cast<T &&>(left)),
                                std::get<Is>(static_cast<U &&>(right))), 42)...);
            }
        };

        template<typename T, typename U, typename = void>
        struct is_swappable_with_
            : std::false_type
        {};

        template<typename T, typename U>
        struct is_swappable_with_<T, U, meta::void_<
            decltype(swap_fn()(std::declval<T>(), std::declval<U>())),
            decltype(swap_fn()(std::declval<U>(), std::declval<T>()))>>
            : std::true_type
        {};

        template<typename T, typename U>
        struct is_nothrow_swappable_with_
            : meta::bool_<noexcept(swap_fn()(std::declval<T>(), std::declval<U>())) &&
                        noexcept(swap_fn()(std::declval<U>(), std::declval<T>()))>
        {};

        // Q: Should std::reference_wrapper be considered a proxy wrt swapping rvalues?
        // A: No. Its operator= is currently defined to reseat the references, so
        //    std::swap(ra, rb) already means something when ra and rb are (lvalue)
        //    reference_wrappers. That reseats the reference wrappers but leaves the
        //    referents unmodified. Treating rvalue reference_wrappers differently would
        //    be confusing.

        // Q: Then why is it OK to "re"-define swap for pairs and tuples of references?
        // A: Because as defined above, swapping an rvalue tuple of references has the same
        //    semantics as swapping an lvalue tuple of references. Rather than reseat the
        //    references, assignment happens *through* the references.

        // Q: But I have an iterator whose operator* returns an rvalue
        //    std::reference_wrapper<T>. How do I make it model IndirectlySwappable?
        // A: With an overload of iter_swap.
    }
    /// \endcond

    /// \ingroup group-utility
    template<typename T, typename U>
    struct is_swappable_with
        : adl_swap_detail::is_swappable_with_<T, U>
    {};

    /// \ingroup group-utility
    template<typename T, typename U>
    struct is_nothrow_swappable_with
        : meta::and_<
            is_swappable_with<T, U>,
            adl_swap_detail::is_nothrow_swappable_with_<T, U>>
    {};

    /// \ingroup group-utility
    template<typename T>
    struct is_swappable
        : is_swappable_with<T &, T &>
    {};

    /// \ingroup group-utility
    template<typename T>
    struct is_nothrow_swappable
        : is_nothrow_swappable_with<T &, T &>
    {};

    /// \ingroup group-utility
    /// \relates adl_swap_detail::swap_fn
    CONCEPTS_DEFINE_CPO(adl_swap_detail::swap_fn, swap)
}

#endif
