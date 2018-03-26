/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct partial_sort_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessIterator<I>() && Sentinel<S, I>())>
            I operator()(I begin, I middle, S end, C pred = C{}, P proj = P{}) const
            {
                make_heap(begin, middle, std::ref(pred), std::ref(proj));
                auto const len = middle - begin;
                I i = middle;
                for(; i != end; ++i)
                {
                    if(invoke(pred, invoke(proj, *i), invoke(proj, *begin)))
                    {
                        iter_swap(i, begin);
                        detail::sift_down_n(begin, len, begin, std::ref(pred), std::ref(proj));
                    }
                }
                sort_heap(begin, middle, std::ref(pred), std::ref(proj));
                return i;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && RandomAccessRange<Rng>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, I middle, C pred = C{},
                P proj = P{}) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `partial_sort_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<partial_sort_fn>,
                               partial_sort)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
