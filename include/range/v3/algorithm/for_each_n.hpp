/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Rostislav Khlebnikov 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_N_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_N_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct for_each_n_fn
    {
        template<typename I, typename F, typename P = identity>
        auto operator()(I begin, iter_difference_t<I> n, F fun, P proj = P{}) const ->
            CPP_ret(I)(
                requires InputIterator<I> && IndirectUnaryInvocable<F, projected<I, P>>)
        {
            RANGES_EXPECT(0 <= n);
            auto norig = n;
            auto b = uncounted(begin);
            for(; 0 < n; ++b, --n)
                invoke(fun, invoke(proj, *b));
            return recounted(begin, b, norig);
        }

        template<typename Rng, typename F, typename P = identity>
        auto operator()(Rng &&rng, range_difference_t<Rng> n, F fun, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires InputRange<Rng> &&
                    IndirectUnaryInvocable<F, projected<iterator_t<Rng>, P>>)
        {
            if (SizedRange<Rng>)
                RANGES_EXPECT(n <= distance(rng));

            return (*this)(begin(rng), n, detail::move(fun), detail::move(proj));
        }
    };

    /// \sa `for_each_n_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(for_each_n_fn, for_each_n)
    /// @}
} // namespace ranges

#endif // include guard
