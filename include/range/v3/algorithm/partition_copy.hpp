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
#ifndef RANGES_V3_ALGORITHM_PARTITION_COPY_HPP
#define RANGES_V3_ALGORITHM_PARTITION_COPY_HPP

#include <tuple>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O0, typename O1>
    using partition_copy_result = detail::in_out1_out2_result<I, O0, O1>;

    struct partition_copy_fn
    {
        template<typename I, typename S, typename O0, typename O1, typename C, typename P = identity>
        auto operator()(I begin, S end, O0 o0, O1 o1, C pred, P proj = P{}) const ->
            CPP_ret(partition_copy_result<I, O0, O1>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    WeaklyIncrementable<O0> &&
                    WeaklyIncrementable<O1> &&
                    IndirectlyCopyable<I, O0> &&
                    IndirectlyCopyable<I, O1> &&
                    IndirectUnaryPredicate<C, projected<I, P>>)
        {
            for(; begin != end; ++begin)
            {
                auto &&x = *begin;
                if(invoke(pred, invoke(proj, x)))
                {
                    *o0 = (decltype(x) &&) x;
                    ++o0;
                }
                else
                {
                    *o1 = (decltype(x) &&) x;
                    ++o1;
                }
            }
            return {begin, o0, o1};
        }

        template<typename Rng, typename O0, typename O1, typename C, typename P = identity>
        auto operator()(Rng &&rng, O0 o0, O1 o1, C pred, P proj = P{}) const ->
            CPP_ret(partition_copy_result<safe_iterator_t<Rng>, O0, O1>)(
                requires InputRange<Rng> &&
                    WeaklyIncrementable<O0> &&
                    WeaklyIncrementable<O1> &&
                    IndirectlyCopyable<iterator_t<Rng>, O0> &&
                    IndirectlyCopyable<iterator_t<Rng>, O1> &&
                    IndirectUnaryPredicate<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(o0), std::move(o1), std::move(pred),
                std::move(proj));
        }
    };

    /// \sa `partition_copy_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(partition_copy_fn, partition_copy)
    /// @}
} // namespace ranges

#endif // include guard
