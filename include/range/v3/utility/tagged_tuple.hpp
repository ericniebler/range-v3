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

#ifndef RANGES_V3_UTILITY_TAGGED_TUPLE_HPP
#define RANGES_V3_UTILITY_TAGGED_TUPLE_HPP

#include <tuple>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/tagged_pair.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename... Ts>
        using tagged_tuple =
            tagged<std::tuple<detail::tag_elem<Ts>...>, detail::tag_spec<Ts>...>;

        template<typename...Tags, typename...Ts>
        constexpr tagged_tuple<Tags(bind_element_t<Ts>)...>
        make_tagged_tuple(Ts &&... ts)
        {
            return tagged_tuple<Tags(bind_element_t<Ts>)...>{static_cast<Ts&&>(ts)...};
        }
    }
}

#endif
