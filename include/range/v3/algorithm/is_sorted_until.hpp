/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
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
#ifndef RANGES_V3_ALGORITHM_IS_SORTED_UNTIL_HPP
#define RANGES_V3_ALGORITHM_IS_SORTED_UNTIL_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct is_sorted_until_fn
    {
        /// \brief template function \c is_sorted_until_fn::operator()
        ///
        /// range-based version of the \c is_sorted_until std algorithm
        ///
        /// Works on forward_ranges
        ///
        /// \pre `Rng` is a model of the `forward_range` concept
        /// \pre `I` is a model of the `forward_iterator` concept
        /// \pre `S` and `I` model the `sentinel_for<S, I>` concept
        /// \pre `R` and `projected<I, P>` model the `indirect_strict_weak_order<R,
        /// projected<I, P>>` concept
        ///
        template<typename I, typename S, typename R = less, typename P = identity>
        auto operator()(I begin, S end, R pred = R{}, P proj = P{}) const
            -> CPP_ret(I)( //
                requires forward_iterator<I> && sentinel_for<S, I> &&
                    indirect_strict_weak_order<R, projected<I, P>>)
        {
            auto i = begin;
            if(begin != end)
            {
                while(++i != end)
                {
                    if(invoke(pred, invoke(proj, *i), invoke(proj, *begin)))
                        return i;
                    begin = i;
                }
            }
            return i;
        }

        template<typename Rng, typename R = less, typename P = identity>
        auto operator()(Rng && rng, R pred = R{}, P proj = P{}) const
            -> CPP_ret(safe_iterator_t<Rng>)( //
                requires forward_range<Rng> &&
                    indirect_strict_weak_order<R, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `is_sorted_until_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(is_sorted_until_fn, is_sorted_until)

    namespace cpp20
    {
        using ranges::is_sorted_until;
    }
    /// @}
} // namespace ranges

#endif // include guard
