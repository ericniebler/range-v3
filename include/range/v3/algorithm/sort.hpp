//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_SORT_HPP
#define RANGES_V3_ALGORITHM_SORT_HPP

#include <range/v3/algorithm/partition.hpp>

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
            void operator()(I begin, I end, C cmp = C{}, P proj = P{}) const
            {
              if (begin != end) {
                I middle = partition(begin, end, [&](auto v) {
                    return cmp(proj(v), proj(*begin));
                });
                I new_middle = begin;
                (*this)(std::move(begin), std::move(middle), std::ref(cmp), std::ref(proj));
                (*this)(++new_middle, std::move(end), std::ref(cmp), std::ref(proj));
              }
            }

          template<typename Rng, typename C = ordered_less, typename P = ident,
                   typename I = range_iterator_t<Rng>,
                   CONCEPT_REQUIRES_(Sortable<I, C, P>())>
            void operator()(Rng & rng, C && cmp = C{}) const
            {
                (*this)(begin(rng), end(rng), std::forward<C>(cmp));
            }
        };

        RANGES_CONSTEXPR sort_fn sort {};

    } // namespace v3
} // namespace ranges

#endif // include guard
