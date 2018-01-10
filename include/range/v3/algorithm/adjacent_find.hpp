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
#ifndef RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct adjacent_find_fn
        {
            /// \brief function template \c adjacent_find_fn::operator()
            ///
            /// range-based version of the \c adjacent_find std algorithm
            ///
            /// \pre `Rng` is a model of the `Range` concept
            /// \pre `C` is a model of the `BinaryPredicate` concept
            template<typename I, typename S, typename C = equal_to, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && Sentinel<S, I>() &&
                    IndirectRelation<C, projected<I, P>>())>
            I
            operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                if(begin == end)
                    return begin;
                auto next = begin;
                for(; ++next != end; begin = next)
                    if(invoke(pred, invoke(proj, *begin), invoke(proj, *next)))
                        return begin;
                return next;
            }

            /// \overload
            template<typename Rng, typename C = equal_to, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(ForwardRange<Rng>() &&
                    IndirectRelation<C, projected<I, P>>())>
            safe_iterator_t<Rng>
            operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `adjacent_find_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<adjacent_find_fn>,
                               adjacent_find)
        /// @}

    } // namespace v3
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
