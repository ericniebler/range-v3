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
            // implementation of logical_and_c and logical_or_c
            std::true_type fast_logical_and_impl_();

            template<typename ...T>
            std::true_type fast_logical_and_impl_(T*...);

            template<typename ...T>
            std::false_type fast_logical_and_impl_(T...);

            std::false_type fast_logical_or_impl_();

            template<typename ...T>
            std::false_type fast_logical_or_impl_(T*...);

            template<typename ...T>
            std::true_type fast_logical_or_impl_(T...);
        }

        template<bool ...Bools>
        using logical_and_c =
            decltype(detail::fast_logical_and_impl_(detail::conditional_t<Bools, int*, int>{}...));

        template<bool ...Bools>
        using logical_or_c =
            decltype(detail::fast_logical_or_impl_(detail::conditional_t<Bools, int, int*>{}...));

        template<bool Bool>
        using logical_not_c = std::integral_constant<bool, !Bool>;

        template<typename...Bools>
        using fast_logical_and = logical_and_c<Bools::value...>;

        template<typename...Bools>
        using fast_logical_or = logical_or_c<Bools::value...>;

        template<typename Bool>
        using logical_not = logical_not_c<Bool::value>;

        template<typename ...Bools>
        struct logical_and;

        template<>
        struct logical_and<>
          : std::true_type
        {};

        template<typename Bool, typename...Bools>
        struct logical_and<Bool, Bools...>
          : detail::conditional_t<!Bool::value, std::false_type, logical_and<Bools...>>
        {};

        template<typename ...Bools>
        struct logical_or;

        template<>
        struct logical_or<>
          : std::false_type
        {};

        template<typename Bool, typename...Bools>
        struct logical_or<Bool, Bools...>
          : detail::conditional_t<Bool::value, std::true_type, logical_and<Bools...>>
        {};


    }
}

#endif
