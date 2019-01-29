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
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct includes_fn
    {
        template<typename I1, typename S1, typename I2, typename S2, typename C = less,
            typename P1 = identity, typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(bool)(
                requires InputIterator<I1> && Sentinel<S1, I1> &&
                    InputIterator<I2> && Sentinel<S2, I2> &&
                    IndirectStrictWeakOrder<C, projected<I1, P1>, projected<I2, P2>>)
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

        template<typename Rng1, typename Rng2, typename C = less, typename P1 = identity,
            typename P2 = identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(bool)(
                requires InputRange<Rng1> && InputRange<Rng2> &&
                    IndirectStrictWeakOrder<C, projected<iterator_t<Rng1>, P1>,
                                               projected<iterator_t<Rng2>, P2>>)
        {
            return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(pred),
                std::move(proj1), std::move(proj2));
        }
    };

    /// \sa `includes_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(includes_fn, includes)

    template<typename I1, typename I2, typename O>
    using set_union_result = detail::in1_in2_out_result<I1, I2, O>;

    struct set_union_fn
    {
        template<typename I1, typename S1, typename I2, typename S2, typename O,
            typename C = less, typename P1 = identity, typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out, C pred = C{},
                P1 proj1 = P1{}, P2 proj2 = P2{}) const ->
            CPP_ret(set_union_result<I1, I2, O>)(
                requires Sentinel<S1, I1> && Sentinel<S2, I2> &&
                    Mergeable<I1, I2, O, C, P1, P2>)
        {
            for(; begin1 != end1; ++out)
            {
                if(begin2 == end2)
                {
                    auto tmp = copy(begin1, end1, out);
                    return {tmp.in, begin2, tmp.out};
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
            return {begin1, tmp.in, tmp.out};
        }

        template<typename Rng1, typename Rng2, typename O, typename C = less,
            typename P1 = identity, typename P2 = identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, O out, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(set_union_result<safe_iterator_t<Rng1>, safe_iterator_t<Rng2>, O>)(
                requires Range<Rng1> && Range<Rng2> &&
                    Mergeable<iterator_t<Rng1>, iterator_t<Rng2>, O, C, P1, P2>)
        {
            return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                std::move(pred), std::move(proj1), std::move(proj2));
        }
    };

    /// \sa `set_union_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(set_union_fn, set_union)

    struct set_intersection_fn
    {
        template<typename I1, typename S1, typename I2, typename S2, typename O,
            typename C = less, typename P1 = identity, typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out, C pred = C{},
                P1 proj1 = P1{}, P2 proj2 = P2{}) const ->
            CPP_ret(O)(
                requires Sentinel<S1, I1> && Sentinel<S2, I2> &&
                    Mergeable<I1, I2, O, C, P1, P2>)
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

        template<typename Rng1, typename Rng2, typename O, typename C = less,
            typename P1 = identity, typename P2 = identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, O out, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(O)(
                requires Range<Rng1> && Range<Rng2> &&
                    Mergeable<iterator_t<Rng1>, iterator_t<Rng2>, O, C, P1, P2>)
        {
            return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                std::move(pred), std::move(proj1), std::move(proj2));
        }
    };

    /// \sa `set_intersection_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(set_intersection_fn, set_intersection)

    template<typename I, typename O>
    using set_difference_result = detail::in1_out_result<I, O>;

    struct set_difference_fn
    {
        template<typename I1, typename S1, typename I2, typename S2, typename O,
            typename C = less, typename P1 = identity, typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out, C pred = C{},
                P1 proj1 = P1{}, P2 proj2 = P2{}) const ->
            CPP_ret(set_difference_result<I1, O>)(
                requires Sentinel<S1, I1> && Sentinel<S2, I2> &&
                    Mergeable<I1, I2, O, C, P1, P2>)
        {
            while(begin1 != end1)
            {
                if(begin2 == end2)
                {
                    auto tmp = copy(begin1, end1, out);
                    return {tmp.in, tmp.out};
                }
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

        template<typename Rng1, typename Rng2, typename O, typename C = less,
            typename P1 = identity, typename P2 = identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, O out, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(set_difference_result<safe_iterator_t<Rng1>, O>)(
                requires Range<Rng1> && Range<Rng2> &&
                    Mergeable<iterator_t<Rng1>, iterator_t<Rng2>, O, C, P1, P2>)
        {
            return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                std::move(pred), std::move(proj1), std::move(proj2));
        }
    };

    /// \sa `set_difference_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(set_difference_fn, set_difference)

    template<typename I1, typename I2, typename O>
    using set_symmetric_difference_result = detail::in1_in2_out_result<I1, I2, O>;

    struct set_symmetric_difference_fn
    {
        template<typename I1, typename S1, typename I2, typename S2, typename O,
            typename C = less, typename P1 = identity, typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, O out, C pred = C{},
                P1 proj1 = P1{}, P2 proj2 = P2{}) const ->
            CPP_ret(set_symmetric_difference_result<I1, I2, O>)(
                requires Sentinel<S1, I1> && Sentinel<S2, I2> &&
                    Mergeable<I1, I2, O, C, P1, P2>)
        {
            while(begin1 != end1)
            {
                if(begin2 == end2)
                {
                    auto tmp = copy(begin1, end1, out);
                    return {tmp.in, begin2, tmp.out};
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
            return {begin1, tmp.in, tmp.out};
        }

        template<typename Rng1, typename Rng2, typename O, typename C = less,
            typename P1 = identity, typename P2 = identity>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, O out, C pred = C{}, P1 proj1 = P1{},
                P2 proj2 = P2{}) const ->
            CPP_ret(set_symmetric_difference_result<safe_iterator_t<Rng1>,
                                                    safe_iterator_t<Rng2>,
                                                    O>)(
                requires Range<Rng1> && Range<Rng2> &&
                    Mergeable<iterator_t<Rng1>, iterator_t<Rng2>, O, C, P1, P2>)
        {
            return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(out),
                std::move(pred), std::move(proj1), std::move(proj2));
        }
    };

    /// \sa `set_symmetric_difference_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(set_symmetric_difference_fn, set_symmetric_difference)
    /// @}
} // namespace ranges

#endif // include guard
