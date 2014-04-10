// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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
            template<typename ...T>
            std::true_type logical_and_impl_(T*...);

            template<typename ...T>
            std::false_type logical_and_impl_(T...);

            template<typename ...T>
            std::false_type logical_or_impl_(T*...);

            template<typename ...T>
            std::true_type logical_or_impl_(T...);
        }

        template<bool ...Bools>
        struct logical_and
          : decltype(detail::logical_and_impl_(
                detail::conditional_t<Bools, int*, int>{}...))
        {};

        template<>
        struct logical_and<>
          : std::true_type
        {};

        template<bool ...Bools>
        struct logical_or
          : decltype(detail::logical_or_impl_(
                detail::conditional_t<Bools, int, int*>{}...))
        {};

        template<>
        struct logical_or<>
          : std::false_type
        {};

        template<typename...Bools>
        using logical_and_t = logical_and<Bools::value...>;

        template<typename...Bools>
        using logical_or_t = logical_or<Bools::value...>;
    }
}

#endif
