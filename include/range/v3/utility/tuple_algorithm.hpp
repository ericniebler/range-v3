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

#ifndef RANGES_V3_UTILITY_TUPLE_ALGORITHM_HPP
#define RANGES_V3_UTILITY_TUPLE_ALGORITHM_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <initializer_list>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        template<typename Tup>
        using tuple_indices_t =
            meta::make_index_sequence<
                std::tuple_size<typename std::remove_reference<Tup>::type>::value>;

        struct tuple_apply_fn
        {
        private:
            template<typename Fun, typename Tup, std::size_t...Is>
            RANGES_RELAXED_CONSTEXPR
            static auto impl(Fun &&fun, Tup &&tup, meta::index_sequence<Is...>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                std::forward<Fun>(fun)(std::get<Is>(std::forward<Tup>(tup))...)
            )
        public:
            template<typename Fun, typename Tup>
            RANGES_RELAXED_CONSTEXPR
            auto operator()(Fun &&fun, Tup &&tup) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                tuple_apply_fn::impl(std::forward<Fun>(fun), std::forward<Tup>(tup),
                    tuple_indices_t<Tup>{})
            )
        };

        /// \ingroup group-utility
        /// \sa `tuple_apply_fn`
        namespace
        {
            constexpr auto&& tuple_apply = static_const<tuple_apply_fn>::value;
        }

        struct tuple_transform_fn
        {
        private:
            template<typename Tup, typename Fun, std::size_t...Is>
            RANGES_RELAXED_CONSTEXPR
            static auto impl1(Tup && tup, Fun fun, meta::index_sequence<Is...>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                std::tuple<decltype(fun(std::get<Is>(std::forward<Tup>(tup))))...>{
                    fun(std::get<Is>(std::forward<Tup>(tup)))...}
            )
            template<typename Tup0, typename Tup1, typename Fun, std::size_t...Is>
            RANGES_RELAXED_CONSTEXPR
            static auto impl2(Tup0 && tup0, Tup1 && tup1, Fun fun, meta::index_sequence<Is...>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                std::tuple<decltype(fun(std::get<Is>(std::forward<Tup0>(tup0)),
                                        std::get<Is>(std::forward<Tup1>(tup1))))...>{
                    fun(std::get<Is>(std::forward<Tup0>(tup0)),
                        std::get<Is>(std::forward<Tup1>(tup1)))...}
            )
        public:
            template<typename Tup, typename Fun>
            RANGES_RELAXED_CONSTEXPR
            auto operator()(Tup && tup, Fun fun) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                tuple_transform_fn::impl1(std::forward<Tup>(tup), std::move(fun),
                    tuple_indices_t<Tup>{})
            )
            template<typename Tup0, typename Tup1, typename Fun>
            RANGES_RELAXED_CONSTEXPR
            auto operator()(Tup0 && tup0, Tup1 && tup1, Fun fun) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                tuple_transform_fn::impl2(std::forward<Tup0>(tup0), std::forward<Tup1>(tup1),
                    std::move(fun), tuple_indices_t<Tup0>{})
            )
        };

        /// \ingroup group-utility
        /// \sa `tuple_transform_fn`
        namespace
        {
            constexpr auto&& tuple_transform = static_const<tuple_transform_fn>::value;
        }

        struct tuple_foldl_fn
        {
        private:
            template<typename Tup, typename Val, typename Fun>
            RANGES_RELAXED_CONSTEXPR
            static Val impl(Tup &&, Val val, Fun)
            {
                return val;
            }
            template<std::size_t I0, std::size_t...Is, typename Tup, typename Val, typename Fun,
                typename Impl = tuple_foldl_fn>
            RANGES_RELAXED_CONSTEXPR
            static auto impl(Tup && tup, Val val, Fun fun)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                Impl::template impl<Is...>(std::forward<Tup>(tup),
                    fun(std::move(val), std::get<I0>(std::forward<Tup>(tup))),
                    std::move(fun))
            )
            template<typename Tup, typename Val, typename Fun, std::size_t...Is>
            RANGES_RELAXED_CONSTEXPR
            static auto impl2(Tup && tup, Val val, Fun fun, meta::index_sequence<Is...>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                tuple_foldl_fn::impl<Is...>(std::forward<Tup>(tup), std::move(val),
                    std::move(fun))
            )
        public:
            template<typename Tup, typename Val, typename Fun>
            RANGES_RELAXED_CONSTEXPR
            auto operator()(Tup && tup, Val val, Fun fun) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                tuple_foldl_fn::impl2(std::forward<Tup>(tup), std::move(val), std::move(fun),
                    tuple_indices_t<Tup>{})
            )
        };

        /// \ingroup group-utility
        /// \sa `tuple_foldl_fn`
        namespace
        {
            constexpr auto&& tuple_foldl = static_const<tuple_foldl_fn>::value;
        }

        struct tuple_for_each_fn
        {
        private:
            template<typename Tup, typename Fun, std::size_t...Is>
            RANGES_RELAXED_CONSTEXPR
            static void impl(Tup && tup, Fun fun, meta::index_sequence<Is...>)
            {
                (void)std::initializer_list<int>{
                    ((void)fun(std::get<Is>(std::forward<Tup>(tup))), 42)...};
            }
        public:
            template<typename Tup, typename Fun>
            RANGES_RELAXED_CONSTEXPR
            Fun operator()(Tup && tup, Fun fun) const
            {
                tuple_for_each_fn::impl(std::forward<Tup>(tup), ranges::ref(fun),
                    tuple_indices_t<Tup>{});
                return fun;
            }
        };

        /// \ingroup group-utility
        /// \sa `tuple_for_each_fn`
        namespace
        {
            constexpr auto&& tuple_for_each = static_const<tuple_for_each_fn>::value;
        }

        struct make_tuple_fn
        {
            template<typename ...Ts>
            RANGES_RELAXED_CONSTEXPR
            auto operator()(Ts &&...ts) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                std::make_tuple(std::forward<Ts>(ts)...)
            )
        };

        /// \ingroup group-utility
        /// \sa `make_tuple_fn`
        namespace
        {
            constexpr auto&& make_tuple = static_const<make_tuple_fn>::value;
        }
        /// @}
    }
}

#endif
