/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_FUNCTIONAL_REFERENCE_WRAPPER_HPP
#define RANGES_V3_FUNCTIONAL_REFERENCE_WRAPPER_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/functional/invoke.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/utility/addressof.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{

    /// \cond
    namespace detail
    {
        template<typename T>
        struct reference_wrapper_
        {
            T * t_ = nullptr;
            constexpr reference_wrapper_() = default;
            constexpr reference_wrapper_(T & t) noexcept
              : t_(detail::addressof(t))
            {}
            constexpr reference_wrapper_(T &&) = delete;
            constexpr T & get() const noexcept
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
            T * t_ = nullptr;
            constexpr reference_wrapper_() = default;
            constexpr reference_wrapper_(T && t) noexcept
              : t_(detail::addressof(t))
            {}
            constexpr T && get() const noexcept
            {
                return static_cast<T &&>(*t_);
            }
        };
    } // namespace detail
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
        constexpr CPP_ctor(reference_wrapper)(U && u)(               //
            noexcept(std::is_nothrow_constructible<base_, U>::value) //
            requires(!defer::same_as<uncvref_t<U>, reference_wrapper>) &&
            defer::constructible_from<base_, U>)
          : detail::reference_wrapper_<T>{static_cast<U &&>(u)}
        {}
#else
        // BUGBUG clang-3.7 prefers a CPP_template here instead of a CPP_ctor
        CPP_template(typename U)( //
            requires(!defer::same_as<uncvref_t<U>, reference_wrapper>) &&
            defer::constructible_from<base_, U>) //
            constexpr reference_wrapper(U && u) noexcept(
                std::is_nothrow_constructible<base_, U>::value)
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
        CPP_template(typename...)(                         //
            requires(!std::is_rvalue_reference<T>::value)) //
        operator std::reference_wrapper<type>() const noexcept
        {
            return {get()};
        }
        // clang-format off
        template<typename ...Args>
        constexpr auto CPP_auto_fun(operator())(Args &&...args) (const)
        (
            return invoke(static_cast<reference>(*t_), static_cast<Args &&>(args)...)
        )
        // clang-format on
    };

    struct ref_fn : pipeable_base
    {
        template<typename T>
        auto operator()(T & t) const -> CPP_ret(reference_wrapper<T>)( //
            requires(!is_reference_wrapper_v<T>))
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

    /// \ingroup group-functional
    /// \sa `ref_fn`
    RANGES_INLINE_VARIABLE(ref_fn, ref)

    template<typename T>
    using ref_t = decltype(ref(std::declval<T>()));

    struct unwrap_reference_fn
    {
        template<typename T>
        T && operator()(T && t) const noexcept
        {
            return static_cast<T &&>(t);
        }
        /// \overload
        template<typename T>
        typename reference_wrapper<T>::reference operator()(reference_wrapper<T> t) const
            noexcept
        {
            return t.get();
        }
        /// \overload
        template<typename T>
        T & operator()(std::reference_wrapper<T> t) const noexcept
        {
            return t.get();
        }
        /// \overload
        template<typename T>
        T & operator()(ref_view<T> t) const noexcept
        {
            return t.base();
        }
    };

    /// \ingroup group-functional
    /// \sa `unwrap_reference_fn`
    RANGES_INLINE_VARIABLE(unwrap_reference_fn, unwrap_reference)

    template<typename T>
    using unwrap_reference_t = decltype(unwrap_reference(std::declval<T>()));
    /// @}
} // namespace ranges

#endif
