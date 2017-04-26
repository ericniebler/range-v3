/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
#include <memory>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
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

        struct invoke_fn
        {
        private:
            template<typename MemberFunctionPtr, typename First, typename... Rest>
            static constexpr auto invoke_member_fn(
                std::true_type, detail::any, MemberFunctionPtr fn, First && first, Rest &&... rest)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                (static_cast<First&&>(first).*fn)(static_cast<Rest&&>(rest)...)
            )
            template<typename MemberFunctionPtr, typename First, typename... Rest>
            static constexpr auto invoke_member_fn(
                std::false_type, std::true_type, MemberFunctionPtr fn, First && first, Rest &&... rest)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                (static_cast<First&&>(first).get().*fn)(static_cast<Rest&&>(rest)...)
            )
            template<typename MemberFunctionPtr, typename First, typename... Rest>
            static constexpr auto invoke_member_fn(
                std::false_type, std::false_type, MemberFunctionPtr fn, First && first, Rest &&... rest)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ((*static_cast<First&&>(first)).*fn)(static_cast<Rest&&>(rest)...)
            )

            template<typename MemberDataPtr, typename First>
            static constexpr auto invoke_member_data(
                std::true_type, detail::any, MemberDataPtr ptr, First && first)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<First&&>(first).*ptr
            )
            template<typename MemberDataPtr, typename First>
            static constexpr auto invoke_member_data(
                std::false_type, std::true_type, MemberDataPtr ptr, First && first)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<First&&>(first).get().*ptr
            )
            template<typename MemberDataPtr, typename First>
            static constexpr auto invoke_member_data(
                std::false_type, std::false_type, MemberDataPtr ptr, First && first)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                (*static_cast<First&&>(first)).*ptr
            )
        public:
            template<typename F, typename Obj, typename First, typename... Rest,
                meta::if_c<detail::is_function<F>::value, int> = 0>
            constexpr auto operator()(F (Obj::*ptr), First && first, Rest &&... rest) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                invoke_member_fn(std::is_base_of<Obj, detail::decay_t<First>>{},
                    is_reference_wrapper_t<detail::decay_t<First>>{},
                    ptr, static_cast<First&&>(first), static_cast<Rest&&>(rest)...)
            )
            template<typename Data, typename Obj, typename First,
                meta::if_c<!detail::is_function<Data>::value, int> = 0>
            constexpr auto operator()(Data (Obj::*ptr), First && first) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                invoke_member_data(std::is_base_of<Obj, detail::decay_t<First>>{},
                    is_reference_wrapper_t<detail::decay_t<First>>{},
                    ptr, static_cast<First&&>(first))
            )
            template<typename F, typename... Args,
                meta::if_c<!std::is_member_pointer<uncvref_t<F>>::value, int> = 0>
            constexpr auto operator()(F && fn, Args &&... args) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<F&&>(fn)(static_cast<Args&&>(args)...)
            )
        };
        RANGES_INLINE_VARIABLE(invoke_fn, invoke)

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
            reference_wrapper(meta::if_c<RValue, T &, T &&>) = delete;
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
                invoke(std::declval<reference>(), std::declval<Args>()...))
            {
                return invoke(get(), static_cast<Args&&>(args)...);
            }
        };

        template<typename Sig, typename Enable /*= void*/>
        struct result_of {};
        template<typename Fun, typename...Args>
        struct result_of<Fun(Args...), meta::void_<
            decltype(invoke(std::declval<Fun>(), std::declval<Args>()...))>>
        {
            using type = decltype(invoke(std::declval<Fun>(), std::declval<Args>()...));
        };
    } // namespace v3
} // namespace ranges

RANGES_DIAGNOSTIC_POP

#endif // RANGES_V3_UTILITY_INVOKE_HPP
