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

#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(for_each_n)

        /// \brief function template \c for_each_n
        template<typename I, typename F, typename P = identity>
        auto RANGES_FUN_NIEBLOID(for_each_n)(
            I first, iter_difference_t<I> n, F fun, P proj = P{}) //
            ->CPP_ret(I)(                                         //
                requires input_iterator<I> &&
                indirectly_unary_invocable<F, projected<I, P>>)
        {
            RANGES_EXPECT(0 <= n);
            auto norig = n;
            auto b = uncounted(first);
            for(; 0 < n; ++b, --n)
                invoke(fun, invoke(proj, *b));
            return recounted(first, b, norig);
        }

        /// \overload
        template<typename Rng, typename F, typename P = identity>
        auto RANGES_FUN_NIEBLOID(for_each_n)(
            Rng && rng, range_difference_t<Rng> n, F fun, P proj = P{})
            ->CPP_ret(safe_iterator_t<Rng>)( //
                requires input_range<Rng> &&
                indirectly_unary_invocable<F, projected<iterator_t<Rng>, P>>)
        {
            if(sized_range<Rng>)
                RANGES_EXPECT(n <= distance(rng));

            return (*this)(begin(rng), n, detail::move(fun), detail::move(proj));
        }

    RANGES_END_NIEBLOID(for_each_n)

    // Not yet!
    //  namespace cpp20
    // {
    //     using ranges::for_each_n;
    // }
    /// @}
} // namespace ranges

#endif // include guard
