// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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
        template<typename Tuple>
        using tuple_indices_t =
            integer_sequence_t<
                std::tuple_size<typename std::remove_reference<Tuple>::type>::value>;

        struct tuple_transform_
        {
        private:
            template<typename Tuple, typename UnaryFunction, std::size_t...Is>
            using unary_result_t = std::tuple<
                result_of_t<UnaryFunction(decltype(std::get<Is>(std::declval<Tuple>())))>...>;
            template<typename Tuple0, typename Tuple1, typename BinaryFunction, std::size_t...Is>
            using binary_result_t = std::tuple<
                result_of_t<BinaryFunction(
                    decltype(std::get<Is>(std::declval<Tuple0>())),
                    decltype(std::get<Is>(std::declval<Tuple1>())))>...>;

            template<typename Tuple, typename UnaryFunction, std::size_t...Is>
            static unary_result_t<Tuple, UnaryFunction, Is...>
            impl(Tuple && tup, UnaryFunction fun, integer_sequence<Is...>)
            {
                return unary_result_t<Tuple, UnaryFunction, Is...>{
                    fun(std::get<Is>(std::forward<Tuple>(tup)))...};
            }
            template<typename Tuple0, typename Tuple1, typename BinaryFunction, std::size_t...Is>
            static binary_result_t<Tuple0, Tuple1, BinaryFunction, Is...>
            impl(Tuple0 && tup0, Tuple1 && tup1, BinaryFunction fun, integer_sequence<Is...>)
            {
                return binary_result_t<Tuple0, Tuple1, BinaryFunction, Is...>{
                    fun(std::get<Is>(std::forward<Tuple0>(tup0)),
                        std::get<Is>(std::forward<Tuple1>(tup1)))...};
            }
        public:
            template<typename Tuple, typename UnaryFunction>
            auto operator()(Tuple && tup, UnaryFunction fun) const ->
                decltype(tuple_transform_::impl(
                    std::declval<Tuple>(),
                    std::declval<UnaryFunction>(),
                    tuple_indices_t<Tuple>{}))
            {
                return tuple_transform_::impl(
                    std::forward<Tuple>(tup),
                    std::move(fun),
                    tuple_indices_t<Tuple>{});
            }
            template<typename Tuple0, typename Tuple1, typename BinaryFunction>
            auto operator()(Tuple0 && tup0, Tuple1 && tup1, BinaryFunction fun) const ->
                decltype(tuple_transform_::impl(
                    std::declval<Tuple0>(),
                    std::declval<Tuple1>(),
                    std::declval<BinaryFunction>(),
                    tuple_indices_t<Tuple0>{}))
            {
                static_assert(
                    std::tuple_size<typename std::remove_reference<Tuple0>::type>::value ==
                    std::tuple_size<typename std::remove_reference<Tuple1>::type>::value,
                    "tuples must be of the same length");
                return tuple_transform_::impl(
                    std::forward<Tuple0>(tup0),
                    std::forward<Tuple1>(tup1),
                    std::move(fun),
                    tuple_indices_t<Tuple0>{});
            }
        };

        RANGES_CONSTEXPR tuple_transform_ tuple_transform {};

        struct tuple_foldl_
        {
        private:
            template<typename Tuple, typename Value, typename BinaryFunction>
            static Value impl(Tuple &&, Value val, BinaryFunction)
            {
                return val;
            }
            template<std::size_t I0, std::size_t...Is,
                     typename Tuple, typename Value, typename BinaryFunction,
                     typename Impl = tuple_foldl_>
            static auto impl(Tuple && tup, Value val, BinaryFunction fun) ->
                decltype(Impl::template impl<Is...>(
                    std::forward<Tuple>(tup),
                    fun(val, std::get<I0>(std::forward<Tuple>(tup))),
                    std::move(fun)))
            {
                auto next_val = fun(std::move(val), std::get<I0>(std::forward<Tuple>(tup)));
                return Impl::template impl<Is...>(
                    std::forward<Tuple>(tup),
                    std::move(next_val),
                    std::move(fun));
            }
            template<typename Tuple, typename Value, typename BinaryFunction, std::size_t...Is>
            static auto impl2(Tuple && tup, Value val, BinaryFunction fun,
                integer_sequence<Is...>) ->
                decltype(tuple_foldl_::impl<Is...>(
                    std::forward<Tuple>(tup),
                    std::move(val),
                    std::move(fun)))
            {
                return tuple_foldl_::impl<Is...>(
                    std::forward<Tuple>(tup),
                    std::move(val),
                    std::move(fun));
            }
        public:
            template<typename Tuple, typename Value, typename BinaryFunction>
            auto operator()(Tuple && tup, Value val, BinaryFunction fun) const ->
                decltype(tuple_foldl_::impl2(
                    std::forward<Tuple>(tup),
                    std::move(val),
                    std::move(fun),
                    tuple_indices_t<Tuple>{}))
            {
                return tuple_foldl_::impl2(
                    std::forward<Tuple>(tup),
                    std::move(val),
                    std::move(fun),
                    tuple_indices_t<Tuple>{});
            }
        };

        RANGES_CONSTEXPR tuple_foldl_ tuple_foldl {};

        // NOTE: This does *not* guarantee order of evaluation, nor does
        // it return the function after it is done. Not to be used with
        // stateful function objects.
        struct tuple_for_each_
        {
        private:
            template<typename...Ts>
            static void ignore(Ts &&...)
            {}
            template<typename Tuple, typename UnaryFunction, std::size_t...Is>
            static void impl(Tuple && tup, UnaryFunction fun, integer_sequence<Is...>)
            {
                return tuple_for_each_::ignore(
                    (static_cast<void>(fun(std::get<Is>(std::forward<Tuple>(tup)))), 42)...);
            }
        public:
            template<typename Tuple, typename UnaryFunction>
            void operator()(Tuple && tup, UnaryFunction fun) const
            {
                return tuple_for_each_::impl(std::forward<Tuple>(tup),
                                             std::move(fun),
                                             tuple_indices_t<Tuple>{});
            }
        };

        RANGES_CONSTEXPR tuple_for_each_ tuple_for_each {};
    }
}

#endif
