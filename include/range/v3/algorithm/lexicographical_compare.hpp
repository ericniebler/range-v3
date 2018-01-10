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
#ifndef RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct lexicographical_compare_fn
        {
            template<typename I0, typename S0, typename I1, typename S1,
                typename C = ordered_less, typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(Sentinel<S0, I0>() && Sentinel<S1, I1>() &&
                    Comparable<I0, I1, C, P0, P1>())>
            bool operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred = C{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                for(; begin1 != end1; ++begin0, ++begin1)
                {
                    if(begin0 == end0 || invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                        return true;
                    if(invoke(pred, invoke(proj1, *begin1), invoke(proj0, *begin0)))
                        return false;
                }
                return false;
            }

            template<typename Rng0, typename Rng1, typename C = ordered_less,
                typename P0 = ident, typename P1 = ident,
                typename I0 = iterator_t<Rng0>,
                typename I1 = iterator_t<Rng1>,
                CONCEPT_REQUIRES_(InputRange<Rng0>() && InputRange<Rng1>() &&
                    Comparable<I0, I1, C, P0, P1>())>
            bool operator()(Rng0 &&rng0, Rng1 &&rng1, C pred = C{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), begin(rng1), end(rng1), std::move(pred),
                    std::move(proj0), std::move(proj1));
            }
        };

        /// \sa `lexicographical_compare_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<lexicographical_compare_fn>,
                               lexicographical_compare)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
