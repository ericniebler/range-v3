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

#ifndef RANGES_V3_UTILITY_COMPRESSED_PAIR_HPP
#define RANGES_V3_UTILITY_COMPRESSED_PAIR_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace compressed_tuple_detail
        {
            // tagging individual elements with the complete type list disambiguates
            // base classes when composing compressed_tuples recursively.
            template<typename T, std::size_t I, typename... Ts>
            using storage = box<T, meta::list<meta::size_t<I>, Ts...>>;

            template<typename List, typename Indices> struct compressed_tuple_;
            template<typename... Ts, std::size_t... Is>
            struct compressed_tuple_<meta::list<Ts...>, meta::index_sequence<Is...>>
              : storage<Ts, Is, Ts...>...
            {
                static_assert(Same<meta::index_sequence<Is...>,
                    meta::make_index_sequence<sizeof...(Is)>>(), "What madness is this?!?");

                compressed_tuple_() = default;

                template<typename... Args,
                    meta::if_<meta::strict_and<std::is_constructible<Ts, Args>...>, int> = 0>
                constexpr compressed_tuple_(Args &&... args)
                    noexcept(meta::strict_and<std::is_nothrow_constructible<storage<Ts, Is, Ts...>, Args>...>::value)
                  : storage<Ts, Is, Ts...>{static_cast<Args&&>(args)}...
                {}

                template<typename... Us,
                    meta::if_<meta::strict_and<std::is_constructible<Us, Ts const &>...>, int> = 0>
                constexpr operator std::tuple<Us...> () const
                    noexcept(meta::strict_and<std::is_nothrow_constructible<Us, Ts const &>...>::value)
                {
                    return std::tuple<Us...>{get<Is>(*this)...};
                }
            };

            template<typename... Ts>
            using compressed_tuple =
                compressed_tuple_<meta::list<Ts...>, meta::make_index_sequence<sizeof...(Ts)>>;

            template<std::size_t I, typename... Ts, std::size_t... Is, typename T = meta::at_c<meta::list<Ts...>, I>>
            RANGES_CXX14_CONSTEXPR T &
            get(compressed_tuple_<meta::list<Ts...>, meta::index_sequence<Is...>> &tuple) noexcept
            {
                return static_cast<storage<T, I, Ts...> &>(tuple).get();
            }
            template<std::size_t I, typename... Ts, std::size_t... Is, typename T = meta::at_c<meta::list<Ts...>, I>>
            constexpr T const &
            get(compressed_tuple_<meta::list<Ts...>, meta::index_sequence<Is...>> const &tuple) noexcept
            {
                return static_cast<storage<T, I, Ts...> const &>(tuple).get();
            }
            template<std::size_t I, typename... Ts, std::size_t... Is, typename T = meta::at_c<meta::list<Ts...>, I>>
            RANGES_CXX14_CONSTEXPR T &&
            get(compressed_tuple_<meta::list<Ts...>, meta::index_sequence<Is...>> &&tuple) noexcept
            {
                return static_cast<storage<T, I, Ts...> &&>(tuple).get();
            }
            template<std::size_t I, typename... Ts, std::size_t... Is, typename T = meta::at_c<meta::list<Ts...>, I>>
            RANGES_CXX14_CONSTEXPR T const &&
            get(compressed_tuple_<meta::list<Ts...>, meta::index_sequence<Is...>> const &&tuple) noexcept
            {
                return static_cast<storage<T, I, Ts...> const &&>(tuple).get();
            }
        }
        /// \endcond

        using compressed_tuple_detail::compressed_tuple;
        using compressed_tuple_detail::get;

        struct make_compressed_tuple_fn
        {
            template<typename... Args>
            constexpr auto operator()(Args &&... args) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
                compressed_tuple<bind_element_t<Args>...>{static_cast<Args&&>(args)...}
            )
        };

        /// \ingroup group-utility
        /// \sa `make_compressed_tuple_fn`
        RANGES_INLINE_VARIABLE(make_compressed_tuple_fn, make_compressed_tuple)

        template<typename... Ts>
        using tagged_compressed_tuple =
            tagged<compressed_tuple<detail::tag_elem<Ts>...>, detail::tag_spec<Ts>...>;

        RANGES_DEFINE_TAG_SPECIFIER(first)
        RANGES_DEFINE_TAG_SPECIFIER(second)

        template<typename First, typename Second>
        struct compressed_pair
            : tagged_compressed_tuple<tag::first(First), tag::second(Second)>
        {
            using base_t = tagged_compressed_tuple<tag::first(First), tag::second(Second)>;
            using first_type = First;
            using second_type = Second;

            using base_t::first;
            using base_t::second;

            compressed_pair() = default;
            using base_t::base_t;

            template<typename F, typename S,
                meta::if_<meta::strict_and<std::is_constructible<F, First const &>,
                                           std::is_constructible<S, Second const &>>, int> = 0>
            constexpr operator std::pair<F, S> () const
                noexcept(std::is_nothrow_constructible<F, First const&>::value &&
                    std::is_nothrow_constructible<S, Second const&>::value)
            {
                return std::pair<F, S>{first(), second()};
            }
        };

        struct make_compressed_pair_fn
        {
            template<typename First, typename Second>
            constexpr auto operator()(First && f, Second && s) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
                compressed_pair<bind_element_t<First>, bind_element_t<Second>>{
                    static_cast<First&&>(f), static_cast<Second&&>(s)
                }
            )
        };

        /// \ingroup group-utility
        /// \sa `make_compressed_pair_fn`
        RANGES_INLINE_VARIABLE(make_compressed_pair_fn, make_compressed_pair)
    }
}

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
namespace std
{
    template<typename... Ts, size_t... Is>
    struct tuple_size< ::ranges::v3::compressed_tuple_detail::compressed_tuple_<::meta::list<Ts...>, ::meta::index_sequence<Is...>>>
      : integral_constant<size_t, sizeof...(Ts)>
    {};

    template<size_t I, typename... Ts, size_t... Is>
    struct tuple_element<I, ::ranges::v3::compressed_tuple_detail::compressed_tuple_<::meta::list<Ts...>, ::meta::index_sequence<Is...>>>
    {
        using type = ::meta::at_c<::meta::list<Ts...>, I>;
    };

    template<typename First, typename Second>
    struct tuple_size< ::ranges::v3::compressed_pair<First, Second>>
      : integral_constant<size_t, 2>
    {};

    template<typename First, typename Second>
    struct tuple_element<0, ::ranges::v3::compressed_pair<First, Second>>
    {
        using type = First;
    };

    template<typename First, typename Second>
    struct tuple_element<1, ::ranges::v3::compressed_pair<First, Second>>
    {
        using type = Second;
    };
}
RANGES_DIAGNOSTIC_POP

#endif
