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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_SET_ALGORITHM_HPP
#define RANGES_V3_ALGORITHM_SET_ALGORITHM_HPP

#include <tuple>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/utility/tagged_tuple.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct includes_fn
        {
            template<typename I1, typename S1, typename I2, typename S2,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Comparable<I1, I2, C, P1, P2>() &&
                    Sentinel<S1, I1>() && Sentinel<S2, I2>())>
            bool operator()(I1 begin1, S1 end1, I2 begin2, S2 end2,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                for(; begin2 != end2; ++begin1)
                {
                    if(begin1 == end1 || invoke(pred, invoke(proj2, *begin2), invoke(proj1, *begin1)))
                        return false;
                    if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                        ++begin2;
                }
                return true;
            }

            template<typename Rng1, typename Rng2, typename C = ordered_less,
                typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Comparable<I1, I2, C, P1, P2>() &&
                    Range<Rng1>() && Range<Rng2>())>
            bool operator()(Rng1 && rng1, Rng2 && rng2,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(pred),
                    std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `includes_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<includes_fn>, includes)

        struct set_union_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Sentinel<S1, I1>() && Sentinel<S2, I2>())>
            tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
            operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                for(; begin1 != end1; ++out)
                {
                    if(begin2 == end2)
                    {
                        auto tmp = copy(begin1, end1, out);
                        return make_tagged_tuple<tag::in1, tag::in2, tag::out>(tmp.first, begin2,
                            tmp.second);
                    }
                    if(invoke(pred, invoke(proj2, *begin2), invoke(proj1, *begin1)))
                    {
                        *out = *begin2;
                        ++begin2;
                    }
                    else
                    {
                        *out = *begin1;
                        if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                            ++begin2;
                        ++begin1;
                    }
                }
                auto tmp = copy(begin2, end2, out);
                return make_tagged_tuple<tag::in1, tag::in2, tag::out>(begin1, tmp.first,
                    tmp.second);
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Range<Rng1>() && Range<Rng2>())>
            tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>), tag::out(O)>
            operator()(Rng1 &&rng1, Rng2 &&rng2, O out, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_union_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<set_union_fn>, set_union)

        struct set_intersection_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Sentinel<S1, I1>() && Sentinel<S2, I2>())>
            O operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                while(begin1 != end1 && begin2 != end2)
                {
                    if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                        ++begin1;
                    else
                    {
                        if(!invoke(pred, invoke(proj2, *begin2), invoke(proj1, *begin1)))
                        {
                            *out = *begin1;
                            ++out;
                            ++begin1;
                        }
                        ++begin2;
                    }
                }
                return out;
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Range<Rng1>() && Range<Rng2>())>
            O operator()(Rng1 && rng1, Rng2 && rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_intersection_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<set_intersection_fn>,
                               set_intersection)

        struct set_difference_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Sentinel<S1, I1>() && Sentinel<S2, I2>())>
            tagged_pair<tag::in1(I1), tag::out(O)> operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                while(begin1 != end1)
                {
                    if(begin2 == end2)
                        return copy(begin1, end1, out);
                    if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                    {
                        *out = *begin1;
                        ++out;
                        ++begin1;
                    }
                    else
                    {
                        if(!invoke(pred, invoke(proj2, *begin2), invoke(proj1, *begin1)))
                            ++begin1;
                        ++begin2;
                    }
                }
                return {begin1, out};
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Range<Rng1>() && Range<Rng2>())>
            tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::out(O)> operator()(Rng1 &&rng1, Rng2 && rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_difference_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<set_difference_fn>,
                               set_difference)

        struct set_symmetric_difference_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Sentinel<S1, I1>() && Sentinel<S2, I2>())>
            tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)> operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                while(begin1 != end1)
                {
                    if(begin2 == end2)
                    {
                        auto tmp = copy(begin1, end1, out);
                        return tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>{tmp.first, begin2, tmp.second};
                    }
                    if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                    {
                        *out = *begin1;
                        ++out;
                        ++begin1;
                    }
                    else
                    {
                        if(invoke(pred, invoke(proj2, *begin2), invoke(proj1, *begin1)))
                        {
                            *out = *begin2;
                            ++out;
                        }
                        else
                            ++begin1;
                        ++begin2;
                    }
                }
                auto tmp = copy(begin2, end2, out);
                return tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>{begin1, tmp.first, tmp.second};
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Range<Rng1>() && Range<Rng2>())>
            tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>), tag::out(O)>
            operator()(Rng1 &&rng1, Rng2 &&rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_symmetric_difference_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<set_symmetric_difference_fn>,
                               set_symmetric_difference)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
