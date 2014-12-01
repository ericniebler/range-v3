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
#include <range/v3/utility/invokable.hpp>
#include <range/v3/algorithm/copy.hpp>

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
                    IteratorRange<I1, S1>() && IteratorRange<I2, S2>())>
            bool operator()(I1 begin1, S1 end1, I2 begin2, S2 end2,
                C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                for(; begin2 != end2; ++begin1)
                {
                    if(begin1 == end1 || pred(proj2(*begin2), proj1(*begin1)))
                        return false;
                    if(!pred(proj1(*begin1), proj2(*begin2)))
                        ++begin2;
                }
                return true;
            }

            template<typename Rng1, typename Rng2, typename C = ordered_less,
                typename P1 = ident, typename P2 = ident,
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = range_iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Comparable<I1, I2, C, P1, P2>() &&
                    Iterable<Rng1>() && Iterable<Rng2>())>
            bool operator()(Rng1 && rng1, Rng2 && rng2,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(pred),
                    std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `includes_fn`
        /// \ingroup group-algorithms
        constexpr with_braced_init_args<includes_fn> includes{};

        struct set_union_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename Tup = std::tuple<I1, I2, O>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    IteratorRange<I1, S1>() && IteratorRange<I2, S2>())>
            Tup operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                for(; begin1 != end1; ++out)
                {
                    if(begin2 == end2)
                    {
                        auto tmp = copy(begin1, end1, out);
                        return Tup{tmp.first, begin2, tmp.second};
                    }
                    if(pred(proj2(*begin2), proj1(*begin1)))
                    {
                        *out = *begin2;
                        ++begin2;
                    }
                    else
                    {
                        *out = *begin1;
                        if(!pred(proj1(*begin1), proj2(*begin2)))
                            ++begin2;
                        ++begin1;
                    }
                }
                auto tmp = copy(begin2, end2, out);
                return Tup{begin1, tmp.first, tmp.second};
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = range_iterator_t<Rng2>,
                typename Tup = std::tuple<I1, I2, O>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Iterable<Rng1 &>() && Iterable<Rng2 &>())>
            Tup operator()(Rng1 & rng1, Rng2 & rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_union_fn`
        /// \ingroup group-algorithms
        constexpr set_union_fn set_union{};

        struct set_intersection_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    IteratorRange<I1, S1>() && IteratorRange<I2, S2>())>
            O operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                while(begin1 != end1 && begin2 != end2)
                {
                    if(pred(proj1(*begin1), proj2(*begin2)))
                        ++begin1;
                    else
                    {
                        if(!pred(proj2(*begin2), proj1(*begin1)))
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
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = range_iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Iterable<Rng1>() && Iterable<Rng2>())>
            O operator()(Rng1 && rng1, Rng2 && rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_intersection_fn`
        /// \ingroup group-algorithms
        constexpr with_braced_init_args<set_intersection_fn> set_intersection{};

        struct set_difference_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    IteratorRange<I1, S1>() && IteratorRange<I2, S2>())>
            std::pair<I1, O> operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                while(begin1 != end1)
                {
                    if(begin2 == end2)
                        return copy(begin1, end1, out);
                    if(pred(proj1(*begin1), proj2(*begin2)))
                    {
                        *out = *begin1;
                        ++out;
                        ++begin1;
                    }
                    else
                    {
                        if(!pred(proj2(*begin2), proj1(*begin1)))
                            ++begin1;
                        ++begin2;
                    }
                }
                return {begin1, out};
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = range_iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Iterable<Rng1 &>() && Iterable<Rng2>())>
            std::pair<I1, O> operator()(Rng1 & rng1, Rng2 && rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_difference_fn`
        /// \ingroup group-algorithms
        constexpr with_braced_init_args<set_difference_fn> set_difference{};

        struct set_symmetric_difference_fn
        {
            template<typename I1, typename S1, typename I2, typename S2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    IteratorRange<I1, S1>() && IteratorRange<I2, S2>())>
            std::tuple<I1, I2, O> operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out,
                C pred_ = C{}, P1 proj1_ = P1{}, P2 proj2_ = P2{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj1 = invokable(proj1_);
                auto &&proj2 = invokable(proj2_);
                while(begin1 != end1)
                {
                    if(begin2 == end2)
                    {
                        auto tmp = copy(begin1, end1, out);
                        return std::tuple<I1, I2, O>{tmp.first, begin2, tmp.second};
                    }
                    if(pred(proj1(*begin1), proj2(*begin2)))
                    {
                        *out = *begin1;
                        ++out;
                        ++begin1;
                    }
                    else
                    {
                        if(pred(proj2(*begin2), proj1(*begin1)))
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
                return std::tuple<I1, I2, O>{begin1, tmp.first, tmp.second};
            }

            template<typename Rng1, typename Rng2, typename O,
                typename C = ordered_less, typename P1 = ident, typename P2 = ident,
                typename I1 = range_iterator_t<Rng1>,
                typename I2 = range_iterator_t<Rng2>,
                CONCEPT_REQUIRES_(Mergeable<I1, I2, O, C, P1, P2>() &&
                    Iterable<Rng1 &>() && Iterable<Rng2 &>())>
            std::tuple<I1, I2, O> operator()(Rng1 & rng1, Rng2 & rng2, O out,
                C pred = C{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                    std::move(pred), std::move(proj1), std::move(proj2));
            }
        };

        /// \sa `set_symmetric_difference_fn`
        /// \ingroup group-algorithms
        constexpr set_symmetric_difference_fn set_symmetric_difference{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
