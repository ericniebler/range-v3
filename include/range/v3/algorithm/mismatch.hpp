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
#ifndef RANGES_V3_ALGORITHM_MISMATCH_HPP
#define RANGES_V3_ALGORITHM_MISMATCH_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        // TODO Would be nice to use Comparable here, but that uses Relation instead
        // of Predicate. Relation requires symmetry: is_valid(pred(a,b)) => is_valid(pred(b,a))

        /// \ingroup group-concepts
        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Mismatchable = meta::strict_and<
            InputIterator<I1>,
            InputIterator<I2>,
            IndirectPredicate<C, projected<I1, P1>, projected<I2, P2>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct mismatch_fn
        {
            template<typename I1, typename S1, typename I2, typename C = equal_to,
                typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mismatchable<I1, I2, C, P1, P2>() && Sentinel<S1, I1>())>
            tagged_pair<tag::in1(I1), tag::in2(I2)>
            operator()(I1 begin1, S1 end1, I2 begin2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const
            {
                for(; begin1 != end1; ++begin1, ++begin2)
                    if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                        break;
                return {begin1, begin2};
            }

            template<typename I1, typename S1, typename I2, typename S2, typename C = equal_to,
                typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mismatchable<I1, I2, C, P1, P2>() && Sentinel<S1, I1>() &&
                    Sentinel<S2, I2>())>
            tagged_pair<tag::in1(I1), tag::in2(I2)>
            operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const
            {
                for(; begin1 != end1 &&  begin2 != end2; ++begin1, ++begin2)
                    if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                        break;
                return {begin1, begin2};
            }

            template<typename Rng1, typename I2Ref, typename C = equal_to, typename P1 = ident,
                typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = uncvref_t<I2Ref>, // [*] See below
                CONCEPT_REQUIRES_(InputRange<Rng1>() && Iterator<I2>() &&
                    Mismatchable<I1, I2, C, P1, P2>())>
            tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(I2)>
            operator()(Rng1 &&rng1, I2Ref &&begin2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), static_cast<I2&&>(begin2), std::move(pred),
                    std::move(proj1), std::move(proj2));
            }

            template<typename Rng1, typename Rng2, typename C = equal_to, typename P1 = ident,
                typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(InputRange<Rng1>() && InputRange<Rng2>() &&
                    Mismatchable<I1, I2, C, P1, P2>())>
            tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>)>
            operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(pred),
                    std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `mismatch_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<mismatch_fn>, mismatch)

        // [*] In this case, the 'begin2' iterator is taken by universal reference. Why? So
        // that we can properly distinguish this case:
        //   int x[] = {1,2,3,4};
        //   int y[] = {1,2,3,4};
        //   mismatch(x, y);
        // Had 'begin2' been taken by value as is customary, this call could match as either
        // two ranges, or a range and an iterator, where the iterator is the array, decayed
        // to a pointer. Yuk!

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
