/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//  Copyright Casey Carter 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/find_if_not.hpp>

/**
 * unstable_remove have O(1) complexity for each element remove, unlike remove O(n) [for worst case]
 * Each erased element overwritten (moved in) with last one. Those, unstable_remove loose elements order.
 */
namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C, typename P = ident>
        using UnstableRemovableIf = meta::strict_and<
                BidirectionalIterator<I>,
                IndirectPredicate<C, projected<I, P>>,
                Permutable<I>>;

        /// \addtogroup group-algorithms
        /// @{
        struct unstable_remove_if_fn
        {
            template <typename I, typename C, typename P = ident,
                    CONCEPT_REQUIRES_(UnstableRemovableIf<I, C, P>())>
            I operator()(I first, I last, C pred, P proj = {}) const
            {
                while(true)
                {
                    first = find_if(std::move(first), last, pred, proj);
                    last  = find_if_not(
                            make_reverse_iterator(std::move(last)),
                            make_reverse_iterator(first),
                            pred, proj).base();
                    if (first == last) return last;
                    *first = iter_move(--last);

                    // discussion here: https://github.com/ericniebler/range-v3/issues/988
                    ++first;
                }
            }

            template<typename Rng, typename C, typename P = ident,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(UnstableRemovableIf<I, C, P>() &&
                                      BidirectionalRange<Rng>() &&
                                      BoundedRange<Rng>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `remove_if_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<unstable_remove_if_fn>, unstable_remove_if)
        /// @}
    }
}

#endif // include guard
