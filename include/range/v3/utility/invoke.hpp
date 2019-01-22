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
#ifndef RANGES_V3_UTILITY_INVOKE_HPP
#define RANGES_V3_UTILITY_INVOKE_HPP

#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS

// constexpr invoke leads to premature function template instantiation
// on clang-3.x
#ifndef RANGES_CONSTEXPR_INVOKE
#if !defined(__clang__) || __clang__ >= 4
#define RANGES_CONSTEXPR_INVOKE 1
#else
#define RANGES_CONSTEXPR_INVOKE 0
#endif
#endif

namespace ranges
{
    /// \addtogroup group-utility
    /// @{


    /// \cond
    namespace detail
    {
        template<typename U>
        U &can_reference_(U &&);

        CPP_def
        (
            template(typename T)
            concept Dereferenceable_,
                requires (T &&t)
                (
                    detail::can_reference_(*static_cast<T &&>(t))
                )
        );

        template<class T>
        /*inline*/ constexpr bool is_reference_wrapper_v =
            meta::is<T, reference_wrapper>::value ||
            meta::is<T, std::reference_wrapper>::value;
    }
    /// \endcond

    template<class T>
    /*inline*/ constexpr bool is_reference_wrapper_v =
        detail::is_reference_wrapper_v<detail::decay_t<T>>;

    template<typename T>
    using is_reference_wrapper = meta::bool_<is_reference_wrapper_v<T>>;

    template<typename T>
    using is_reference_wrapper_t RANGES_DEPRECATED("is_reference_wrapper_t is deprecated.") =
        meta::_t<is_reference_wrapper<T>>;

    struct invoke_fn
    {
    private:
        template<class, class T1>
        constexpr static decltype(auto) CPP_fun(coerce)(T1&& t1, long)(
            noexcept(noexcept(*static_cast<T1&&>(t1)))
                requires detail::Dereferenceable_<T1>)
        {
            return *static_cast<T1&&>(t1);
        }

        template<class T, class T1>
        constexpr static auto coerce(T1&& t1, int) noexcept ->
            CPP_ret(T1 &&)(
                requires DerivedFrom<detail::decay_t<T1>, T>)
        {
            return static_cast<T1&&>(t1);
        }

        template<class, class T1>
        constexpr static decltype(auto) CPP_fun(coerce)(T1&& t1, int)(
            noexcept(true)
            requires detail::is_reference_wrapper_v<detail::decay_t<T1>>)
        {
            return static_cast<T1&&>(t1).get();
        }
    public:
        template<class F, class T, class T1, class... Args>
        constexpr auto CPP_auto_fun(operator())(F T::*f, T1&& t1, Args&&... args)(const)
        (
            return (invoke_fn::coerce<T>(static_cast<T1&&>(t1), 0).*f)(static_cast<Args&&>(args)...)
        )

        template<class D, class T, class T1>
        constexpr auto CPP_auto_fun(operator())(D T::*f, T1&& t1)(const)
        (
            return invoke_fn::coerce<T>(static_cast<T1&&>(t1), 0).*f
        )

        template<class F, class... Args>
        CPP_PP_IIF(RANGES_CONSTEXPR_INVOKE)(CPP_PP_EXPAND, CPP_PP_EAT)(constexpr)
        auto CPP_auto_fun(operator())(F&& f, Args&&... args)(const)
        (
            return static_cast<F&&>(f)(static_cast<Args&&>(args)...)
        )
    };

    RANGES_INLINE_VARIABLE(invoke_fn, invoke)

    /// \cond
    namespace detail
    {
        template<typename T>
        struct reference_wrapper_
        {
            T *t_ = nullptr;
            constexpr reference_wrapper_() = default;
            constexpr reference_wrapper_(T &t) noexcept
              : t_(std::addressof(t))
            {}
            constexpr reference_wrapper_(T &&) = delete;
            constexpr T &get() const noexcept
            {
                return *t_;
            }
        };
        template<typename T>
        struct reference_wrapper_<T &> : reference_wrapper_<T>
        {
            using reference_wrapper_<T>::reference_wrapper_;
        };
        template<typename T>
        struct reference_wrapper_<T &&>
        {
            T *t_ = nullptr;
            constexpr reference_wrapper_() = default;
            constexpr reference_wrapper_(T &&t) noexcept
              : t_(std::addressof(t))
            {}
            constexpr T &&get() const noexcept
            {
                return static_cast<T &&>(*t_);
            }
        };
    }
    /// \endcond

    // Can be used to store rvalue references in addition to lvalue references.
    // Also, see: https://wg21.link/lwg2993
    template<typename T>
    struct reference_wrapper : private detail::reference_wrapper_<T>
    {
    private:
        using base_ = detail::reference_wrapper_<T>;
        using base_::t_;
    public:
        using type = meta::_t<std::remove_reference<T>>;
        using reference = meta::if_<std::is_reference<T>, T, T &>;

        constexpr reference_wrapper() = default;
#if !defined(__clang__) || __clang_major__ > 3
        template<typename U>
        constexpr CPP_ctor(reference_wrapper)(U &&u)(
            noexcept(std::is_nothrow_constructible<base_, U>::value)
            requires not defer::Same<uncvref_t<U>, reference_wrapper> &&
                defer::Constructible<base_, U>)
            : detail::reference_wrapper_<T>{static_cast<U &&>(u)}
        {}
#else
        // BUGBUG clang-3.7 prefers a CPP_template here instead of a CPP_ctor
        CPP_template(typename U)(
            requires not defer::Same<uncvref_t<U>, reference_wrapper> &&
                defer::Constructible<base_, U>)
        constexpr reference_wrapper(U &&u)
            noexcept(std::is_nothrow_constructible<base_, U>::value)
          : detail::reference_wrapper_<T>{static_cast<U &&>(u)}
        {}
#endif
        constexpr reference get() const noexcept
        {
            return this->base_::get();
        }
        constexpr operator reference() const noexcept
        {
            return get();
        }
        CPP_template(typename...)(
            requires not std::is_rvalue_reference<T>())
        operator std::reference_wrapper<type> () const noexcept
        {
            return {get()};
        }
        template<typename ...Args>
        constexpr auto CPP_auto_fun(operator())(Args &&...args) (const)
        (
            return invoke(static_cast<reference>(*t_), static_cast<Args &&>(args)...)
        )
    };

#ifdef RANGES_WORKAROUND_MSVC_701385
    /// \cond
    namespace detail
    {
        template<typename Void, typename Fun, typename...Args>
        struct _invoke_result_
        {};

        template<typename Fun, typename...Args>
        struct _invoke_result_<
            meta::void_<decltype(invoke(std::declval<Fun>(), std::declval<Args>()...))>,
            Fun, Args...>
        {
            using type = decltype(invoke(std::declval<Fun>(), std::declval<Args>()...));
        };
    }
    /// \endcond

    template<typename Fun, typename...Args>
    using invoke_result = detail::_invoke_result_<void, Fun, Args...>;

    template<typename Fun, typename...Args>
    using invoke_result_t = meta::_t<invoke_result<Fun, Args...>>;

#else // RANGES_WORKAROUND_MSVC_701385
    template<typename Fun, typename...Args>
    using invoke_result_t =
        decltype(invoke(std::declval<Fun>(), std::declval<Args>()...));

    template<typename Fun, typename...Args>
    struct invoke_result
      : meta::defer<invoke_result_t, Fun, Args...>
    {};
#endif // RANGES_WORKAROUND_MSVC_701385

    template<typename Sig>
    struct RANGES_DEPRECATED("ranges::result_of is deprecated. "
                             "Please use ranges::invoke_result") result_of
    {};

    template<typename Fun, typename...Args>
    struct RANGES_DEPRECATED("ranges::result_of is deprecated. "
                             "Please use ranges::invoke_result") result_of<Fun(Args...)>
      : meta::defer<invoke_result_t, Fun, Args...>
    {};
} // namespace ranges

RANGES_DIAGNOSTIC_POP

#endif // RANGES_V3_UTILITY_INVOKE_HPP
