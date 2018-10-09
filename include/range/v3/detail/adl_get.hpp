/// \file
// Range v3 library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef RANGES_V3_DETAIL_ADL_GET_HPP
#define RANGES_V3_DETAIL_ADL_GET_HPP

#include <cstddef>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            namespace _adl_get_
            {
                template<typename> void get();

                template<std::size_t I, typename TupleLike>
                constexpr auto adl_get(TupleLike &&t) noexcept ->
                    decltype(get<I>(static_cast<TupleLike &&>(t)))
                {
                    return get<I>(static_cast<TupleLike &&>(t));
                }
                template<typename T, typename TupleLike>
                constexpr auto adl_get(TupleLike &&t) noexcept ->
                    decltype(get<T>(static_cast<TupleLike &&>(t)))
                {
                    return get<T>(static_cast<TupleLike &&>(t));
                }
            }
            using _adl_get_::adl_get;
        }
    }
}

#endif // RANGES_V3_DETAIL_ADL_GET_HPP
