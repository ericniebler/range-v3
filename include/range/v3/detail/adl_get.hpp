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
                template<class> void get();

                template<std::size_t I, typename Tuple>
                constexpr auto adl_get(Tuple &&t)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    get<I>(static_cast<Tuple &&>(t))
                )

                template<typename T, typename Tuple>
                constexpr auto adl_get(Tuple &&t)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    get<T>(static_cast<Tuple &&>(t))
                )
            }
            using _adl_get_::adl_get;
        }
    }
}

#endif // RANGES_V3_DETAIL_ADL_GET_HPP
