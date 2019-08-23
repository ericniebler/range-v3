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

#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/unreachable_sentinel.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-numerics
    /// @{
    // clang-format off
    CPP_def
    (
        template(typename I, typename O, typename BOp = minus, typename P = identity)
        (concept differenceable)(I, O, BOp, P),
            input_iterator<I> &&
            invocable<P&, iter_value_t<I>> &&
            copy_constructible<uncvref_t<invoke_result_t<P&, iter_value_t<I>>>> &&
            movable<uncvref_t<invoke_result_t<P&, iter_value_t<I>>>> &&
            output_iterator<O, invoke_result_t<P&, iter_value_t<I>>> &&
            invocable<BOp&, invoke_result_t<P&, iter_value_t<I>>, invoke_result_t<P&, iter_value_t<I>>> &&
            output_iterator<O, invoke_result_t<BOp&, invoke_result_t<P&, iter_value_t<I>>, invoke_result_t<P&, iter_value_t<I>>>>
    );
    // clang-format on

    template<typename I, typename O>
    using adjacent_difference_result = detail::in_out_result<I, O>;

    struct adjacent_difference_fn
    {
        template<typename I, typename S, typename O, typename S2, typename BOp = minus,
                 typename P = identity>
        auto operator()(I first, S last, O result, S2 end_result, BOp bop = BOp{},
                        P proj = P{}) const
            -> CPP_ret(adjacent_difference_result<I, O>)( //
                requires sentinel_for<S, I> && sentinel_for<S2, O> &&
                    differenceable<I, O, BOp, P>)
        {
            // BUGBUG think about the use of coerce here.
            using V = iter_value_t<I>;
            using X = invoke_result_t<P &, V>;
            coerce<V> v;
            coerce<X> x;

            if(first != last && result != end_result)
            {
                auto t1(x(invoke(proj, v(*first))));
                *result = t1;
                for(++first, ++result; first != last && result != end_result;
                    ++first, ++result)
                {
                    auto t2(x(invoke(proj, v(*first))));
                    *result = invoke(bop, t2, t1);
                    t1 = std::move(t2);
                }
            }
            return {first, result};
        }

        template<typename I, typename S, typename O, typename BOp = minus,
                 typename P = identity>
        auto operator()(I first, S last, O result, BOp bop = BOp{}, P proj = P{}) const
            -> CPP_ret(adjacent_difference_result<I, O>)( //
                requires sentinel_for<S, I> && differenceable<I, O, BOp, P>)
        {
            return (*this)(std::move(first),
                           std::move(last),
                           std::move(result),
                           unreachable,
                           std::move(bop),
                           std::move(proj));
        }

        template<typename Rng, typename ORef, typename BOp = minus, typename P = identity,
                 typename I = iterator_t<Rng>, typename O = uncvref_t<ORef>>
        auto operator()(Rng && rng, ORef && result, BOp bop = BOp{}, P proj = P{}) const
            -> CPP_ret(adjacent_difference_result<safe_iterator_t<Rng>, O>)( //
                requires range<Rng> && differenceable<I, O, BOp, P>)
        {
            return (*this)(begin(rng),
                           end(rng),
                           static_cast<ORef &&>(result),
                           std::move(bop),
                           std::move(proj));
        }

        template<typename Rng, typename ORng, typename BOp = minus, typename P = identity,
                 typename I = iterator_t<Rng>, typename O = iterator_t<ORng>>
        auto operator()(Rng && rng, ORng && result, BOp bop = BOp{}, P proj = P{}) const
            -> CPP_ret(adjacent_difference_result<safe_iterator_t<Rng>,
                                                  safe_iterator_t<ORng>>)( //
                requires range<Rng> && range<ORng> && differenceable<I, O, BOp, P>)
        {
            return (*this)(begin(rng),
                           end(rng),
                           begin(result),
                           end(result),
                           std::move(bop),
                           std::move(proj));
        }
    };

    RANGES_INLINE_VARIABLE(adjacent_difference_fn, adjacent_difference)
    /// @}
} // namespace ranges

#endif
