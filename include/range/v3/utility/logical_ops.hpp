// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_LOGICAL_OPS_HPP
#define RANGES_V3_UTILITY_LOGICAL_OPS_HPP

#include <type_traits>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // Thanks to  Louis Dionne for this clever hack for a quick-to-compile
            // implementation of logical_and and logical_or
            std::true_type logical_and_impl_();

            template<typename ...T>
            std::true_type logical_and_impl_(T*...);

            template<typename ...T>
            std::false_type logical_and_impl_(T...);

            std::false_type logical_or_impl_();

            template<typename ...T>
            std::false_type logical_or_impl_(T*...);

            template<typename ...T>
            std::true_type logical_or_impl_(T...);
        }

        template<bool ...Bools>
        using logical_and =
            decltype(detail::logical_and_impl_(detail::conditional_t<Bools, int*, int>{}...));

        template<bool ...Bools>
        using logical_or =
            decltype(detail::logical_or_impl_(detail::conditional_t<Bools, int, int*>{}...));

        template<bool Bool>
        using logical_not = std::integral_constant<bool, !Bool>;

        template<typename...Bools>
        using logical_and_t = logical_and<Bools::value...>;

        template<typename...Bools>
        using logical_or_t = logical_or<Bools::value...>;

        template<typename Bool>
        using logical_not_t = logical_not<Bool::value>;
    }
}

#endif
