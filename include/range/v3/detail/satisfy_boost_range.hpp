/// \file
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

#ifndef RANGES_V3_DETAIL_SATISFY_BOOST_RANGE_HPP
#define RANGES_V3_DETAIL_SATISFY_BOOST_RANGE_HPP

#include <range/v3/range_fwd.hpp>

namespace boost {

	/// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
	template<typename T, typename U> struct range_mutable_iterator;

	/// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
	template<typename T, typename U> struct range_const_iterator;

	/// \brief Boost Range specialisation point, for making Boost ranges out of range-v3 views
	template<typename T            > struct range_value;
}

/// \brief Macro specialising Boost Range metafunctions for the specified view
#define RANGES_SATISFY_BOOST_RANGE(view_name) \
namespace boost {                                                                               \
                                                                                                \
    /* Provide a specialisation of boost::range_mutable_iterator for the view that's       */   \
    /* enabled when the view (with const both added and removed) is a ranges::BoundedRange */   \
    template <typename... Ts>                                                                   \
    struct range_mutable_iterator<                                                              \
        view_name< Ts... >,                                                                     \
        ::meta::if_c<                                                                           \
            ::ranges::BoundedRange< typename std::add_const   < view_name< Ts... > >::type >()  \
            &&                                                                                  \
            ::ranges::BoundedRange< typename std::remove_const< view_name< Ts... > >::type >()  \
        >                                                                                       \
    > {                                                                                         \
        using type = ::ranges::range_iterator_t< view_name< Ts... >       >;                    \
    };                                                                                          \
                                                                                                \
    /* Provide a specialisation of boost::range_const_iterator for the view that's         */   \
    /* enabled when the view (with const both added and removed) is a ranges::BoundedRange */   \
    template <typename... Ts>                                                                   \
    struct range_const_iterator<                                                                \
        view_name< Ts... >,                                                                     \
        ::meta::if_c<                                                                           \
            ::ranges::BoundedRange< typename std::add_const   < view_name< Ts... > >::type >()  \
            &&                                                                                  \
            ::ranges::BoundedRange< typename std::remove_const< view_name< Ts... > >::type >()  \
        >                                                                                       \
    > {                                                                                         \
        using type = ::ranges::range_iterator_t< view_name< Ts... > const >;                    \
    };                                                                                          \
                                                                                                \
    /* Provide a specialisation of boost::range_value for the view */                           \
    template <typename... Ts>                                                                   \
    struct range_value< view_name< Ts... > > {                                                  \
        using type = ranges::range_value_t< view_name< Ts... > >;                               \
    };                                                                                          \
}

#endif
