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
#ifndef RANGES_V3_ALGORITHM_SORT_HPP
#define RANGES_V3_ALGORITHM_SORT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct sort_n_fn
        {
            template<typename I, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    Sortable<I, C, P>()
                )>
            void operator()(I begin, iterator_difference_t<I> n, C cmp = C{}, P proj = P{})
            {

            }
        };

        RANGES_CONSTEXPR sort_n_fn sort_n {};

        struct sort_fn
        {
            template<typename I, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    Sortable<I, C, P>()
                )>
            void operator()(I begin, I end, C cmp = C{}, P proj = P{})
            {

            }
        };

        RANGES_CONSTEXPR sort_fn sort {};

    } // namespace v3
} // namespace ranges

#endif // include guard
