// Range v3 library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef RANGES_V3_DETAIL_ADL_GET_HPP
#define RANGES_V3_DETAIL_ADL_GET_HPP

#include <cstddef>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        namespace _adl_get_
        {
            template<typename> void get();

            template<std::size_t I, typename TupleLike>
            constexpr auto adl_get(TupleLike &&t) noexcept ->
                decltype(get<I>(static_cast<TupleLike &&>(t)))
            {
                return get<I>(static_cast<TupleLike &&>(t));
            }
            template<typename T, typename TupleLike>
            constexpr auto adl_get(TupleLike &&t) noexcept ->
                decltype(get<T>(static_cast<TupleLike &&>(t)))
            {
                return get<T>(static_cast<TupleLike &&>(t));
            }
        }
        using _adl_get_::adl_get;
    }

    namespace _tuple_wrapper_
    {
        template<typename TupleLike>
        struct forward_tuple_interface : TupleLike
        {
            forward_tuple_interface() = default;
            using TupleLike::TupleLike;
#if !defined(__clang__) || __clang_major__ > 3
            CPP_member
            constexpr CPP_ctor(forward_tuple_interface)(TupleLike&& base)(
                noexcept(std::is_nothrow_move_constructible<TupleLike>::value)
                requires MoveConstructible<TupleLike>)
              : TupleLike(static_cast<TupleLike&&>(base))
            {}
            CPP_member
            constexpr CPP_ctor(forward_tuple_interface)(TupleLike const& base)(
                noexcept(std::is_nothrow_copy_constructible<TupleLike>::value)
                requires CopyConstructible<TupleLike>)
              : TupleLike(base)
            {}
#else
            // Clang 3.x have a problem with inheriting constructors
            // that causes the declarations in the preceeding PP block to get
            // instantiated too early.
            CPP_template(typename B = TupleLike)(
                requires MoveConstructible<B>)
            constexpr forward_tuple_interface(TupleLike&& base)
                noexcept(std::is_nothrow_move_constructible<TupleLike>::value)
              : TupleLike(static_cast<TupleLike&&>(base))
            {}
            CPP_template(typename B = TupleLike)(
                requires CopyConstructible<B>)
            constexpr forward_tuple_interface(TupleLike const& base)
                noexcept(std::is_nothrow_copy_constructible<TupleLike>::value)
              : TupleLike(base)
            {}
#endif
        };

        template<std::size_t I, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> &wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<I>(static_cast<TupleLike &>(wb))
        )
        template<std::size_t I, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> const &wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<I>(static_cast<TupleLike const &>(wb))
        )
        template<std::size_t I, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> &&wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<I>(static_cast<TupleLike &&>(wb))
        )
        template<std::size_t I, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> const &&wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<I>(static_cast<TupleLike const &&>(wb))
        )
        template<typename T, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> &wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<T>(static_cast<TupleLike &>(wb))
        )
        template<typename T, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> const &wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<T>(static_cast<TupleLike const &>(wb))
        )
        template<typename T, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> &&wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<T>(static_cast<TupleLike &&>(wb))
        )
        template<typename T, typename TupleLike>
        constexpr auto get(forward_tuple_interface<TupleLike> const &&wb)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::adl_get<T>(static_cast<TupleLike const &&>(wb))
        )
    }
    /// \endcond
}

#endif // RANGES_V3_DETAIL_ADL_GET_HPP
