/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct max_element_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && Sentinel<S, I>() &&
                    IndirectRelation<C, projected<I, P>>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                if(begin != end)
                    for(auto tmp = next(begin); tmp != end; ++tmp)
                        if(invoke(pred, invoke(proj, *begin), invoke(proj, *tmp)))
                            begin = tmp;
                return begin;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(ForwardRange<Rng>() &&
                    IndirectRelation<C, projected<I, P>>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `max_element_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<max_element_fn>, max_element)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
