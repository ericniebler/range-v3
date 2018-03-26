/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 0.0. (See accompanying
//  file LICENSE_0_0.txt or copy at
//  http://www.boost.org/LICENSE_0_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP
#define RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP

#include <utility>
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
        struct find_first_of_fn
        {
            // Rationale: return I0 instead of pair<I0,I1> because find_first_of need
            // not actually compute the end of [I1,S0); therefore, it is not necessarily
            // losing information. E.g., if begin0 == end0, we can return begin0 immediately.
            // If we returned pair<I0,I1>, we would need to do an O(N) scan to find the
            // end position.
            template<typename I0, typename S0, typename I1, typename S1,
                     typename R = equal_to, typename P0 = ident, typename P1 = ident,
                     CONCEPT_REQUIRES_(Sentinel<S0, I0>() && Sentinel<S1, I1>() &&
                        ForwardIterator<I1>() && AsymmetricallyComparable<I0, I1, R, P0, P1>())>
            I0 operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, R pred = R{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                for(; begin0 != end0; ++begin0)
                    for(auto tmp = begin1; tmp != end1; ++tmp)
                        if(invoke(pred, invoke(proj0, *begin0), invoke(proj1, *tmp)))
                            return begin0;
                return begin0;
            }

            template<typename Rng0, typename Rng1, typename R = equal_to,
                     typename P0 = ident, typename P1 = ident,
                     typename I0 = iterator_t<Rng0>,
                     typename I1 = iterator_t<Rng1>,
                     CONCEPT_REQUIRES_(Range<Rng0>() && Range<Rng1>() &&
                        ForwardIterator<I1>() && AsymmetricallyComparable<I0, I1, R, P0, P1>())>
            safe_iterator_t<Rng0> operator()(Rng0 &&rng0, Rng1 &&rng1, R pred = R{}, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), begin(rng1), end(rng1), std::move(pred),
                    std::move(proj0), std::move(proj1));
            }
        };

        /// \sa `find_first_of_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<find_first_of_fn>,
                               find_first_of)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
