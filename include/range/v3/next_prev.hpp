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

#ifndef RANGES_V3_NEXT_PREV_HPP
#define RANGES_V3_NEXT_PREV_HPP

#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct nexter : bindable<nexter>
        {
            template<typename Iter>
            static Iter invoke(nexter, Iter it, iterator_difference_t<Iter> n = 1)
            {
                std::advance(it, n);
                return it;
            }
        };

        struct prever : bindable<prever>
        {
            template<typename Iter>
            static Iter invoke(prever, Iter it, iterator_difference_t<Iter> n = 1)
            {
                std::advance(it, -n);
                return it;
            }
        };

        RANGES_CONSTEXPR nexter next {};
        RANGES_CONSTEXPR prever prev {};
    }
}

#endif
