/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DETAIL_SATISFY_BOOST_RANGE_HPP
#define RANGES_V3_DETAIL_SATISFY_BOOST_RANGE_HPP

#include <range/v3/range_fwd.hpp>
#include <meta/meta.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

namespace boost
{
    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
    template<typename T, typename U> struct range_mutable_iterator;

    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
    template<typename T, typename U> struct range_const_iterator;

    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
    template<typename T> struct range_value;

    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
    template<typename T> struct range_size;
}

/// \brief Macro specialising Boost Range metafunctions for the specified view
#define RANGES_SATISFY_BOOST_RANGE(view_name)                                   \
namespace boost                                                                 \
{                                                                               \
    template<typename... Ts>                                                    \
    struct range_mutable_iterator<                                              \
        view_name<Ts...>,                                                       \
        ::meta::if_c<(bool)::ranges::BoundedRange<view_name<Ts...>>()>>         \
    {                                                                           \
        using type = ::ranges::iterator_t<view_name<Ts...>>;                    \
    };                                                                          \
    template<typename... Ts>                                                    \
    struct range_const_iterator<                                                \
        view_name<Ts...>,                                                       \
        ::meta::if_c<(bool)::ranges::BoundedRange<view_name<Ts...> const>()>>   \
    {                                                                           \
        using type = ::ranges::iterator_t<view_name<Ts...> const>;              \
    };                                                                          \
    template<typename... Ts>                                                    \
    struct range_value<view_name<Ts...>>                                        \
    {                                                                           \
        using type = ::ranges::range_value_type_t<view_name<Ts...>>;            \
    };                                                                          \
    template<typename... Ts>                                                    \
    struct range_size<view_name<Ts...>>                                         \
      : ::meta::if_c<                                                           \
            (bool)::ranges::BoundedRange<view_name<Ts...>>(),                   \
            ::meta::defer<::ranges::range_size_type_t, view_name<Ts...>>,       \
            ::meta::nil_>                                                       \
    {                                                                           \
    };                                                                          \
    template<typename... Ts>                                                    \
    struct range_size<view_name<Ts...> const>                                   \
      : ::meta::if_c<                                                           \
            (bool)::ranges::BoundedRange<view_name<Ts...> const>(),             \
            ::meta::defer<::ranges::range_size_type_t, view_name<Ts...> const>, \
            ::meta::nil_>                                                       \
    {                                                                           \
    };                                                                          \
}

#endif
