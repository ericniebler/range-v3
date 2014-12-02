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
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/integer_sequence.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        template<typename Tup>
        using tuple_indices_t =
            make_index_sequence<
                std::tuple_size<typename std::remove_reference<Tup>::type>::value>;

        struct tuple_transform_fn
        {
        private:
            template<typename Tup, typename Fun, std::size_t...Is>
            using unary_result_t = std::tuple<
                result_of_t<Fun(decltype(std::get<Is>(std::declval<Tup>())))>...>;
            template<typename Tup0, typename Tup1, typename Fun, std::size_t...Is>
            using binary_result_t = std::tuple<
                result_of_t<Fun(
                    decltype(std::get<Is>(std::declval<Tup0>())),
                    decltype(std::get<Is>(std::declval<Tup1>())))>...>;

            template<typename Tup, typename Fun, std::size_t...Is>
            static unary_result_t<Tup, Fun, Is...>
            impl1(Tup && tup, Fun fun, index_sequence<Is...>)
            {
                return unary_result_t<Tup, Fun, Is...>{
                    fun(std::get<Is>(std::forward<Tup>(tup)))...};
            }
            template<typename Tup0, typename Tup1, typename Fun, std::size_t...Is>
            static binary_result_t<Tup0, Tup1, Fun, Is...>
            impl2(Tup0 && tup0, Tup1 && tup1, Fun fun, index_sequence<Is...>)
            {
                return binary_result_t<Tup0, Tup1, Fun, Is...>{
                    fun(std::get<Is>(std::forward<Tup0>(tup0)),
                        std::get<Is>(std::forward<Tup1>(tup1)))...};
            }
        public:
            template<typename Tup, typename Fun>
            auto operator()(Tup && tup, Fun fun) const ->
                decltype(tuple_transform_fn::impl1(
                    std::declval<Tup>(),
                    std::declval<Fun>(),
                    tuple_indices_t<Tup>{}))
            {
                return tuple_transform_fn::impl1(
                    std::forward<Tup>(tup),
                    std::move(fun),
                    tuple_indices_t<Tup>{});
            }
            template<typename Tup0, typename Tup1, typename Fun>
            auto operator()(Tup0 && tup0, Tup1 && tup1, Fun fun) const ->
                decltype(tuple_transform_fn::impl2(
                    std::declval<Tup0>(),
                    std::declval<Tup1>(),
                    std::declval<Fun>(),
                    tuple_indices_t<Tup0>{}))
            {
                static_assert(
                    std::tuple_size<typename std::remove_reference<Tup0>::type>::value ==
                    std::tuple_size<typename std::remove_reference<Tup1>::type>::value,
                    "tuples must be of the same length");
                return tuple_transform_fn::impl2(
                    std::forward<Tup0>(tup0),
                    std::forward<Tup1>(tup1),
                    std::move(fun),
                    tuple_indices_t<Tup0>{});
            }
        };

        /// \ingroup group-utility
        /// \sa `tuple_transform_fn`
        constexpr tuple_transform_fn tuple_transform {};

        struct tuple_foldl_fn
        {
        private:
            template<typename Tup, typename Val, typename Fun>
            static Val impl(Tup &&, Val val, Fun)
            {
                return val;
            }
            template<std::size_t I0, std::size_t...Is,
                     typename Tup, typename Val, typename Fun,
                     typename Impl = tuple_foldl_fn>
            static auto impl(Tup && tup, Val val, Fun fun) ->
                decltype(Impl::template impl<Is...>(
                    std::forward<Tup>(tup),
                    fun(val, std::get<I0>(std::forward<Tup>(tup))),
                    std::move(fun)))
            {
                auto next_val = fun(std::move(val), std::get<I0>(std::forward<Tup>(tup)));
                return Impl::template impl<Is...>(
                    std::forward<Tup>(tup),
                    std::move(next_val),
                    std::move(fun));
            }
            template<typename Tup, typename Val, typename Fun, std::size_t...Is>
            static auto impl2(Tup && tup, Val val, Fun fun, index_sequence<Is...>) ->
                decltype(tuple_foldl_fn::impl<Is...>(
                    std::forward<Tup>(tup),
                    std::move(val),
                    std::move(fun)))
            {
                return tuple_foldl_fn::impl<Is...>(
                    std::forward<Tup>(tup),
                    std::move(val),
                    std::move(fun));
            }
        public:
            template<typename Tup, typename Val, typename Fun>
            auto operator()(Tup && tup, Val val, Fun fun) const ->
                decltype(tuple_foldl_fn::impl2(
                    std::forward<Tup>(tup),
                    std::move(val),
                    std::move(fun),
                    tuple_indices_t<Tup>{}))
            {
                return tuple_foldl_fn::impl2(
                    std::forward<Tup>(tup),
                    std::move(val),
                    std::move(fun),
                    tuple_indices_t<Tup>{});
            }
        };

        /// \ingroup group-utility
        /// \sa `tuple_foldl_fn`
        constexpr tuple_foldl_fn tuple_foldl {};

        // NOTE: This does *not* guarantee order of evaluation, nor does
        // it return the function after it is done. Not to be used with
        // stateful function objects.
        struct tuple_for_each_fn
        {
        private:
            template<typename...Ts>
            static void ignore(Ts &&...)
            {}
            template<typename Tup, typename Fun, std::size_t...Is>
            static void impl(Tup && tup, Fun fun, index_sequence<Is...>)
            {
                tuple_for_each_fn::ignore(
                    (static_cast<void>(fun(std::get<Is>(std::forward<Tup>(tup)))), 42)...);
            }
        public:
            template<typename Tup, typename Fun>
            void operator()(Tup && tup, Fun fun) const
            {
                tuple_for_each_fn::impl(std::forward<Tup>(tup),
                                      std::move(fun),
                                      tuple_indices_t<Tup>{});
            }
        };

        /// \ingroup group-utility
        /// \sa `tuple_for_each_fn`
        constexpr tuple_for_each_fn tuple_for_each {};

        struct tuple_apply_fn
        {
        private:
            template<typename Fun, typename Tup, std::size_t...Is>
            static auto impl(Fun &&fun, Tup &&tup, index_sequence<Is...>) ->
                decltype(std::forward<Fun>(fun)(std::get<Is>(std::forward<Tup>(tup))...))
            {
                return std::forward<Fun>(fun)(std::get<Is>(std::forward<Tup>(tup))...);
            }
        public:
            template<typename Fun, typename Tup>
            auto operator()(Fun &&fun, Tup &&tup) const ->
                decltype(tuple_apply_fn::impl(std::forward<Fun>(fun),
                                              std::forward<Tup>(tup),
                                              tuple_indices_t<Tup>{}))
            {
                return tuple_apply_fn::impl(std::forward<Fun>(fun),
                                            std::forward<Tup>(tup),
                                            tuple_indices_t<Tup>{});
            }
        };

        /// \ingroup group-utility
        /// \sa `tuple_apply_fn`
        constexpr tuple_apply_fn tuple_apply {};

        struct make_tuple_fn
        {
            template<typename ...Ts>
            std::tuple<Ts...> operator()(Ts &&...ts) const
            {
                return std::tuple<Ts...>{std::forward<Ts>(ts)...};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_tuple_fn`
        constexpr make_tuple_fn make_tuple {};
        /// @}
    }
}

#endif
