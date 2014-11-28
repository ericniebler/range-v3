// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements: Thanks for Paul Fultz for the suggestions that
//                   concepts can be ordinary Boolean metafunctions.
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_SWAP_HPP
#define RANGES_V3_UTILITY_SWAP_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_swap_detail
        {
            using std::swap;
            struct swap_fn
            {
                template<typename T, typename U>
                auto operator()(T && t, U && u) const
                    noexcept(noexcept(swap(std::declval<T>(), std::declval<U>()))) ->
                    decltype(swap(std::declval<T>(), std::declval<U>()))
                {
                    swap(std::forward<T>(t), std::forward<U>(u));
                }
            };
        }

        constexpr adl_swap_detail::swap_fn swap {};
    }
}

#endif
