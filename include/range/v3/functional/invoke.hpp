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
#ifndef RANGES_V3_FUNCTIONAL_INVOKE_HPP
#define RANGES_V3_FUNCTIONAL_INVOKE_HPP

#include <functional>
#include <type_traits>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/utility/static_const.hpp>

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS

#ifndef RANGES_CONSTEXPR_INVOKE
#ifdef RANGES_WORKAROUND_CLANG_23135
#define RANGES_CONSTEXPR_INVOKE 0
#else
#define RANGES_CONSTEXPR_INVOKE 1
#endif
#endif

namespace ranges
{
    /// \addtogroup group-functional
    /// @{

    /// \cond
    namespace detail
    {
        template<typename U>
        U & can_reference_(U &&);

        // clang-format off
        CPP_def
        (
            template(typename T)
            concept dereferenceable_,
                requires (T &&t)
                (
                    detail::can_reference_(*static_cast<T &&>(t))
                )
        );
        // clang-format on

        template<class T>
        RANGES_INLINE_VAR constexpr bool is_reference_wrapper_v =
            meta::is<T, reference_wrapper>::value ||
            meta::is<T, std::reference_wrapper>::value;
    } // namespace detail
    /// \endcond

    template<class T>
    RANGES_INLINE_VAR constexpr bool is_reference_wrapper_v =
        detail::is_reference_wrapper_v<detail::decay_t<T>>;

    template<typename T>
    using is_reference_wrapper = meta::bool_<is_reference_wrapper_v<T>>;

    template<typename T>
    using is_reference_wrapper_t RANGES_DEPRECATED(
        "is_reference_wrapper_t is deprecated.") = meta::_t<is_reference_wrapper<T>>;

    struct invoke_fn
    {
    private:
        template<class, class T1>
        constexpr static decltype(auto) CPP_fun(coerce)(T1 && t1, long)( //
            noexcept(noexcept(*static_cast<T1 &&>(t1)))                  //
            requires detail::dereferenceable_<T1>)
        {
            return *static_cast<T1 &&>(t1);
        }

        template<class T, class T1>
        constexpr static auto coerce(T1 && t1, int) noexcept -> CPP_ret(T1 &&)( //
            requires derived_from<detail::decay_t<T1>, T>)
        {
            return static_cast<T1 &&>(t1);
        }

        template<class, class T1>
        constexpr static decltype(auto) CPP_fun(coerce)(T1 && t1, int)(
            noexcept(true) //
            requires detail::is_reference_wrapper_v<detail::decay_t<T1>>)
        {
            return static_cast<T1 &&>(t1).get();
        }

    public:
        // clang-format off
        template<class F, class T, class T1, class... Args>
        constexpr auto CPP_auto_fun(operator())(F T::*f, T1&& t1, Args&&... args)(const)
        (
            return (invoke_fn::coerce<T>(static_cast<T1&&>(t1), 0).*f)
                (static_cast<Args&&>(args)...)
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
        // clang-format on
    };

    RANGES_INLINE_VARIABLE(invoke_fn, invoke)

#ifdef RANGES_WORKAROUND_MSVC_701385
    /// \cond
    namespace detail
    {
        template<typename Void, typename Fun, typename... Args>
        struct _invoke_result_
        {};

        template<typename Fun, typename... Args>
        struct _invoke_result_<
            meta::void_<decltype(invoke(std::declval<Fun>(), std::declval<Args>()...))>,
            Fun, Args...>
        {
            using type = decltype(invoke(std::declval<Fun>(), std::declval<Args>()...));
        };
    } // namespace detail
    /// \endcond

    template<typename Fun, typename... Args>
    using invoke_result = detail::_invoke_result_<void, Fun, Args...>;

    template<typename Fun, typename... Args>
    using invoke_result_t = meta::_t<invoke_result<Fun, Args...>>;

#else  // RANGES_WORKAROUND_MSVC_701385
    template<typename Fun, typename... Args>
    using invoke_result_t =
        decltype(invoke(std::declval<Fun>(), std::declval<Args>()...));

    template<typename Fun, typename... Args>
    struct invoke_result : meta::defer<invoke_result_t, Fun, Args...>
    {};
#endif // RANGES_WORKAROUND_MSVC_701385

    /// \cond
    namespace detail
    {
        template<bool IsInvocable>
        struct is_nothrow_invocable_impl_
        {
            template<typename Fn, typename... Args>
            static constexpr bool apply() noexcept
            {
                return false;
            }
        };
        template<>
        struct is_nothrow_invocable_impl_<true>
        {
            template<typename Fn, typename... Args>
            static constexpr bool apply() noexcept
            {
                return noexcept(invoke(std::declval<Fn>(), std::declval<Args>()...));
            }
        };
    } // namespace detail
    /// \endcond

    template<typename Fn, typename... Args>
    RANGES_INLINE_VAR constexpr bool is_invocable_v =
        meta::is_trait<invoke_result<Fn, Args...>>::value;

    template<typename Fn, typename... Args>
    RANGES_INLINE_VAR constexpr bool is_nothrow_invocable_v =
        detail::is_nothrow_invocable_impl_<is_invocable_v<Fn, Args...>>::template apply<
            Fn, Args...>();

    /// \cond
    template<typename Sig>
    struct RANGES_DEPRECATED(
        "ranges::result_of is deprecated. "
        "Please use ranges::invoke_result") result_of
    {};

    template<typename Fun, typename... Args>
    struct RANGES_DEPRECATED(
        "ranges::result_of is deprecated. "
        "Please use ranges::invoke_result") result_of<Fun(Args...)>
      : meta::defer<invoke_result_t, Fun, Args...>
    {};
    /// \endcond

    namespace cpp20
    {
        using ranges::invoke;
        using ranges::invoke_result;
        using ranges::invoke_result_t;
        using ranges::is_invocable_v;
        using ranges::is_nothrow_invocable_v;
    } // namespace cpp20

    /// @}
} // namespace ranges

RANGES_DIAGNOSTIC_POP

#endif // RANGES_V3_FUNCTIONAL_INVOKE_HPP
