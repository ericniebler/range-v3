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
#ifndef RANGES_V3_NUMERIC_ACCUMULATE_HPP
#define RANGES_V3_NUMERIC_ACCUMULATE_HPP

#include <meta/meta.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    CPP_def
    (
        template(typename I, typename T, typename Op = plus, typename P = identity)
        (concept Accumulateable)(I, T, Op, P),
            InputIterator<I> &&
            IndirectBinaryInvocable_<Op, T *, projected<I, P>> &&
            Assignable<T&, indirect_result_t<Op &, T *, projected<I, P>>>
    );

    struct accumulate_fn
    {
        CPP_template(typename I, typename S, typename T, typename Op = plus, typename P = identity)(
            requires Sentinel<S, I> && Accumulateable<I, T, Op, P>)
        T operator()(I begin, S end, T init, Op op = Op{}, P proj = P{}) const
        {
            for(; begin != end; ++begin)
                init = invoke(op, init, invoke(proj, *begin));
            return init;
        }

        CPP_template(typename Rng, typename T, typename Op = plus, typename P = identity)(
            requires Range<Rng> && Accumulateable<iterator_t<Rng>, T, Op, P>)
        T operator()(Rng &&rng, T init, Op op = Op{}, P proj = P{}) const
        {
            return (*this)(begin(rng), end(rng), std::move(init), std::move(op),
                std::move(proj));
        }
    };

    RANGES_INLINE_VARIABLE(accumulate_fn, accumulate)
}

#endif
