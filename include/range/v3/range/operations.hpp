/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Gonzalo Brito Gadeschi 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_RANGE_OPERATIONS_HPP
#define RANGES_V3_RANGE_OPERATIONS_HPP

#include <stdexcept>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// Checked indexed range access.
    ///
    /// \ingroup group-core
    struct at_fn
    {
        /// \return `begin(rng)[n]`
        template<typename Rng, typename Int>
        constexpr /*c++14*/ auto operator()(Rng &&rng, Int n) const ->
            CPP_ret(range_reference_t<Rng>)(
                requires RandomAccessRange<Rng> && SizedRange<Rng> && Integral<Int>)
        {
            using D = range_difference_t<Rng>;
            // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67371 in GCC 5
            check_throw(rng, static_cast<D>(n));
            return ranges::begin(rng)[static_cast<D>(n)];
        }

    private:
        template<typename Rng>
        static constexpr /*c++14*/ void check_throw(Rng &&rng, range_difference_t<Rng> n)
        {
            (n < 0 || n >= ranges::distance(rng))
                ? throw std::out_of_range("ranges::at")
                : void(0);
        }
    };

    /// Checked indexed range access.
    ///
    /// \ingroup group-core
    /// \sa `at_fn`
    RANGES_INLINE_VARIABLE(at_fn, at)

    /// Unchecked indexed range access.
    ///
    /// \ingroup group-core
    struct index_fn
    {
        /// \return `begin(rng)[n]`
        template<typename Rng, typename Int>
        constexpr /*c++14*/ auto operator()(Rng &&rng, Int n) const ->
            CPP_ret(range_reference_t<Rng>)(
                requires RandomAccessRange<Rng> && Integral<Int>)
        {
            using D = range_difference_t<Rng>;
            RANGES_EXPECT(0 <= static_cast<D>(n));
            RANGES_EXPECT(!(bool)SizedRange<Rng> || static_cast<D>(n) < ranges::distance(rng));
            return ranges::begin(rng)[static_cast<D>(n)];
        }
    };

    /// Unchecked indexed range access.
    ///
    /// \ingroup group-core
    /// \sa `index_fn`
    RANGES_INLINE_VARIABLE(index_fn, index)

    /// \ingroup group-core
    struct back_fn
    {
        /// \return `*prev(end(rng))`
        template<typename Rng>
        constexpr /*c++14*/ auto operator()(Rng &&rng) const ->
            CPP_ret(range_reference_t<Rng>)(
                requires CommonRange<Rng> && BidirectionalRange<Rng>)
        {
            return *prev(end(rng));
        }
    };

    /// \ingroup group-core
    /// \sa `back_fn`
    RANGES_INLINE_VARIABLE(back_fn, back)

    /// \ingroup group-core
    struct front_fn
    {
        /// \return `*begin(rng)`
        template<typename Rng>
        constexpr auto operator()(Rng &&rng) const ->
            CPP_ret(range_reference_t<Rng>)(
                requires ForwardRange<Rng>)
        {
            return *begin(rng);
        }
    };

    /// \ingroup group-core
    /// \sa `front_fn`
    RANGES_INLINE_VARIABLE(front_fn, front)
}

#endif
