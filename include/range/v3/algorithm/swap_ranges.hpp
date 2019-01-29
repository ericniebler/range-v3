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
#ifndef RANGES_V3_ALGORITHM_SWAP_RANGES_HPP
#define RANGES_V3_ALGORITHM_SWAP_RANGES_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I1, typename I2>
    using swap_ranges_result = detail::in1_in2_result<I1, I2>;

    struct swap_ranges_fn
    {
        template<typename I1, typename S1, typename I2>
        auto operator()(I1 begin1, S1 end1, I2 begin2) const ->
            CPP_ret(swap_ranges_result<I1, I2>)(
                requires InputIterator<I1> && Sentinel<S1, I1> && InputIterator<I2> &&
                    IndirectlySwappable<I1, I2>)
        {
            for(; begin1 != end1; ++begin1, ++begin2)
                ranges::iter_swap(begin1, begin2);
            return {begin1, begin2};
        }

        template<typename I1, typename S1, typename I2, typename S2>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2) const ->
            CPP_ret(swap_ranges_result<I1, I2>)(
                requires InputIterator<I1> && Sentinel<S1, I1> && InputIterator<I2> &&
                    Sentinel<S2, I2> && IndirectlySwappable<I1, I2>)
        {
            for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                ranges::iter_swap(begin1, begin2);
            return {begin1, begin2};
        }

        template<typename Rng1, typename I2_>
        auto operator()(Rng1 &&rng1, I2_ &&begin2) const ->
            CPP_ret(swap_ranges_result<iterator_t<Rng1>, uncvref_t<I2_>>)(
                requires InputRange<Rng1> && InputIterator<uncvref_t<I2_>> &&
                    IndirectlySwappable<iterator_t<Rng1>, uncvref_t<I2_>>)
        {
            return (*this)(begin(rng1), end(rng1), (I2_ &&)begin2);
        }

        template<typename Rng1, typename Rng2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2) const ->
            CPP_ret(swap_ranges_result<safe_iterator_t<Rng1>, safe_iterator_t<Rng2>>)(
                requires InputRange<Rng1> && InputRange<Rng2> &&
                    IndirectlySwappable<iterator_t<Rng1>, iterator_t<Rng2>>)
        {
            return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2));
        }
    };

    /// \sa `swap_ranges_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(swap_ranges_fn, swap_ranges)
    /// @}
} // namespace ranges

#endif // include guard
