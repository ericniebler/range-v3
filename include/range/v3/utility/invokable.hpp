// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_INVOKABLE_HPP
#define RANGES_V3_UTILITY_INVOKABLE_HPP

#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        RANGES_CONSTEXPR struct invokable_maker
        {
            template<typename R, typename T>
            auto operator()(R T::* p) const ->
                decltype(std::mem_fn(p))
            {
                return std::mem_fn(p);
            }

            template<typename T, typename U = typename std::decay<T>::type>
            auto operator()(T && t) const ->
                typename std::enable_if<!std::is_member_pointer<U>::value, U>::type
            {
                return detail::forward<T>(t);
            }
        } make_invokable{};
    }
}

#endif // RANGES_V3_UTILITY_INVOKABLE_HPP
