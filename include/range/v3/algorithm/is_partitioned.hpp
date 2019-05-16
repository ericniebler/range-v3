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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_IS_PARTITIONED_HPP
#define RANGES_V3_ALGORITHM_IS_PARTITIONED_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct is_partitioned_fn
    {
        template<typename I, typename S, typename C, typename P = identity>
        auto operator()(I begin, S end, C pred, P proj = P{}) const ->
            CPP_ret(bool)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    IndirectUnaryPredicate<C, projected<I, P>>)
        {
            for(; begin != end; ++begin)
                if(!invoke(pred, invoke(proj, *begin)))
                    break;
            for(; begin != end; ++begin)
                if(invoke(pred, invoke(proj, *begin)))
                    return false;
            return true;
        }

        template<typename Rng, typename C, typename P = identity>
        auto operator()(Rng &&rng, C pred, P proj = P{}) const ->
            CPP_ret(bool)(
                requires InputRange<Rng> &&
                    IndirectUnaryPredicate<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `is_partitioned_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(is_partitioned_fn, is_partitioned)

    namespace cpp20
    {
        using ranges::is_partitioned;
    }
    /// @}
} // namespace ranges

#endif // include guard
