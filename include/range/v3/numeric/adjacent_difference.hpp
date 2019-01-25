/// \file
// Range v3 library
//
// Copyright Eric Niebler 2004
// Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define RANGES_V3_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <meta/meta.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/iterator/unreachable_sentinel.hpp>

namespace ranges
{
    CPP_def
    (
        template(typename I, typename O, typename BOp = minus, typename P = identity)
        (concept AdjacentDifferentiable)(I, O, BOp, P),
            InputIterator<I> &&
            Invocable<P&, iter_value_t<I>> &&
            CopyConstructible<uncvref_t<invoke_result_t<P&, iter_value_t<I>>>> &&
            Movable<uncvref_t<invoke_result_t<P&, iter_value_t<I>>>> &&
            OutputIterator<O, invoke_result_t<P&, iter_value_t<I>>> &&
            Invocable<BOp&, invoke_result_t<P&, iter_value_t<I>>, invoke_result_t<P&, iter_value_t<I>>> &&
            OutputIterator<O, invoke_result_t<BOp&, invoke_result_t<P&, iter_value_t<I>>, invoke_result_t<P&, iter_value_t<I>>>>
    );

    template<typename I, typename O>
    using adjacent_difference_result = detail::in_out_result<I, O>;

    struct adjacent_difference_fn
    {
        CPP_template(typename I, typename S, typename O, typename S2,
            typename BOp = minus, typename P = identity)(
            requires Sentinel<S, I> && Sentinel<S2, O> &&
                AdjacentDifferentiable<I, O, BOp, P>)
        adjacent_difference_result<I, O>
        operator()(I begin, S end, O result, S2 end_result, BOp bop = BOp{},
                   P proj = P{}) const
        {
            // BUGBUG think about the use of coerce here.
            using V = iter_value_t<I>;
            using X = invoke_result_t<P&, V>;
            coerce<V> v;
            coerce<X> x;

            if(begin != end && result != end_result)
            {
                auto t1(x(invoke(proj, v(*begin))));
                *result = t1;
                for(++begin, ++result; begin != end && result != end_result;
                    ++begin, ++result)
                {
                    auto t2(x(invoke(proj, v(*begin))));
                    *result = invoke(bop, t2, t1);
                    t1 = std::move(t2);
                }
            }
            return {begin, result};
        }

        CPP_template(typename I, typename S, typename O, typename BOp = minus,
            typename P = identity)(
            requires Sentinel<S, I> &&
                AdjacentDifferentiable<I, O, BOp, P>)
        adjacent_difference_result<I, O>
        operator()(I begin, S end, O result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(std::move(begin), std::move(end), std::move(result),
                           unreachable, std::move(bop), std::move(proj));
        }

        CPP_template(typename Rng, typename ORef, typename BOp = minus, typename P = identity,
            typename I = iterator_t<Rng>, typename O = uncvref_t<ORef>)(
            requires Range<Rng> &&
                AdjacentDifferentiable<I, O, BOp, P>)
        adjacent_difference_result<safe_iterator_t<Rng>, O>
        operator()(Rng &&rng, ORef &&result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(begin(rng), end(rng), static_cast<ORef &&>(result), std::move(bop),
                           std::move(proj));
        }

        CPP_template(typename Rng, typename ORng, typename BOp = minus,
            typename P = identity, typename I = iterator_t<Rng>,
            typename O = iterator_t<ORng>)(
            requires Range<Rng> && Range<ORng> &&
                AdjacentDifferentiable<I, O, BOp, P>)
        adjacent_difference_result<safe_iterator_t<Rng>, safe_iterator_t<ORng>>
        operator()(Rng &&rng, ORng &&result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(begin(rng), end(rng), begin(result), end(result),
                           std::move(bop), std::move(proj));
        }
    };

    RANGES_INLINE_VARIABLE(adjacent_difference_fn, adjacent_difference)
}

#endif
