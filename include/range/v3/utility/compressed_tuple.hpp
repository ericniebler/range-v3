// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_COMPRESSED_TUPLE_HPP
#define RANGES_V3_UTILITY_COMPRESSED_TUPLE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/integer_sequence.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Indices, typename...Ts>
            struct compressed_tuple_data;

            template<std::size_t...Is, typename...Ts>
            struct compressed_tuple_data<index_sequence<Is...>, Ts...>
              : box<Ts, std::integral_constant<std::size_t, Is>>...
            {
                constexpr compressed_tuple_data() = default;
                template<typename...Us,
                         typename = decltype(detail::valid_exprs(Ts{std::declval<Us>()}...))>
                explicit constexpr compressed_tuple_data(Us &&...us)
                  : box<Ts, std::integral_constant<std::size_t, Is>>{detail::forward<Us>(us)}...
                {}
            };

            struct compressed_tuple_core_access
            {
                template<typename Tuple>
                static constexpr auto get_data(Tuple && tup) ->
                    decltype((std::forward<Tuple>(tup).data_))
                {
                    return detail::forward<Tuple>(tup).data_;
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename... Ts>
        struct compressed_tuple
        {
            constexpr compressed_tuple() = default;
            template<typename...Us,
                     typename = decltype(detail::valid_exprs(Ts{std::declval<Us>()}...))>
            explicit constexpr compressed_tuple(Us &&...us)
              : data_{detail::forward<Us>(us)...}
            {}
        private:
            friend struct detail::compressed_tuple_core_access;
            detail::compressed_tuple_data<make_index_sequence<sizeof...(Ts)>, Ts...> data_;
        };

        // Get by index
        template<std::size_t I, typename ...Ts>
        auto get(compressed_tuple<Ts...> & tup) ->
            decltype(ranges::get<I>(detail::compressed_tuple_core_access::get_data(tup)))
        {
            return ranges::get<I>(detail::compressed_tuple_core_access::get_data(tup));
        }

        template<std::size_t I, typename ...Ts>
        constexpr auto get(compressed_tuple<Ts...> const & tup) ->
            decltype(ranges::get<I>(detail::compressed_tuple_core_access::get_data(tup)))
        {
            return ranges::get<I>(detail::compressed_tuple_core_access::get_data(tup));
        }

        template<std::size_t I, typename ...Ts>
        constexpr auto get(compressed_tuple<Ts...> && tup) ->
            decltype(ranges::get<I>(detail::compressed_tuple_core_access::get_data(detail::move(tup))))
        {
            return ranges::get<I>(detail::compressed_tuple_core_access::get_data(detail::move(tup)));
        }

        struct make_compressed_tuple_fn
        {
            template<typename...Ts>
            constexpr auto operator()(Ts &&... ts) const ->
                compressed_tuple<Ts...>
            {
                return compressed_tuple<Ts...>{detail::forward<Ts>(ts)...};
            }
        };
        
        /// \ingroup group-utility
        /// \sa `make_compressed_tuple_fn`
        constexpr make_compressed_tuple_fn make_compressed_tuple {};

        /// @}
    }
}

#endif
