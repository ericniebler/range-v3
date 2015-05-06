/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
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

namespace ranges
{
    inline namespace v3
    {
        // TODO Would be nice to use WeaklyComparable here, but that uses Relation instead
        // of Predicate. Relation requires symmetry: is_valid(pred(a,b)) => is_valid(pred(b,a))

        /// \ingroup group-concepts
        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Mismatchable1 = meta::fast_and<
            InputIterator<I1>,
            WeakInputIterator<I2>,
            IndirectCallablePredicate<C, Project<I1, P1>, Project<I2, P2>>>;

        /// \ingroup group-concepts
        template<typename I1, typename I2, typename C = equal_to, typename P1 = ident,
            typename P2 = ident>
        using Mismatchable2 = meta::fast_and<
            InputIterator<I1>,
            InputIterator<I2>,
            IndirectCallablePredicate<C, Project<I1, P1>, Project<I2, P2>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct mismatch_fn
        {
            template<typename I1, typename S1, typename I2, typename C = equal_to,
                typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mismatchable1<I1, I2, C, P1, P2>() && IteratorRange<I1, S1>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<I1, I2>
            operator()(I1 begin1, S1 end1, I2 begin2, C pred_ = C{}, P1 proj1_ = P1{},
                P2 proj2_ = P2{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj1 = as_function(proj1_);
                auto &&proj2 = as_function(proj2_);
                for(; begin1 != end1; ++begin1, ++begin2)
                    if(!pred(proj1(*begin1), proj2(*begin2)))
                        break;
                return {begin1, begin2};
            }

            template<typename I1, typename S1, typename I2, typename S2, typename C = equal_to,
                typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mismatchable2<I1, I2, C, P1, P2>() && IteratorRange<I1, S1>() &&
                    IteratorRange<I2, S2>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<I1, I2>
            operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, C pred_ = C{}, P1 proj1_ = P1{},
                P2 proj2_ = P2{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj1 = as_function(proj1_);
                auto &&proj2 = as_function(proj2_);
                for(; begin1 != end1 &&  begin2 != end2; ++begin1, ++begin2)
                    if(!pred(proj1(*begin1), proj2(*begin2)))
                        break;
                return {begin1, begin2};
            }

            template<typename Rng1, typename I2Ref, typename C = equal_to, typename P1 = ident,
                typename P2 = ident,
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = uncvref_t<I2Ref>, // [*] See below
                CONCEPT_REQUIRES_(InputIterable<Rng1>() && Iterator<I2>() &&
                    Mismatchable1<I1, I2, C, P1, P2>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<range_safe_iterator_t<Rng1>, I2>
            operator()(Rng1 &&rng1, I2Ref &&begin2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), std::forward<I2>(begin2), std::move(pred),
                    std::move(proj1), std::move(proj2));
            }

            template<typename Rng1, typename Rng2, typename C = equal_to, typename P1 = ident,
                typename P2 = ident,
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = range_iterator_t<Rng2>,
                CONCEPT_REQUIRES_(InputIterable<Rng1>() && InputIterable<Rng2>() &&
                    Mismatchable2<I1, I2, C, P1, P2>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<range_safe_iterator_t<Rng1>, range_safe_iterator_t<Rng2>>
            operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(pred),
                    std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `mismatch_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& mismatch = static_const<with_braced_init_args<mismatch_fn>>::value;
        }

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
