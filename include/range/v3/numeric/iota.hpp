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
#ifndef RANGES_V3_NUMERIC_IOTA_HPP
#define RANGES_V3_NUMERIC_IOTA_HPP

#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-numerics
    /// @{
    struct iota_fn
    {
        template<typename O, typename S, typename T>
        auto operator()(O begin, S end, T val) const -> CPP_ret(O)( //
            requires OutputIterator<O, T const &> && Sentinel<S, O> &&
                WeaklyIncrementable<T>)
        {
            for(; begin != end; ++begin, ++val)
                *begin = detail::as_const(val);
            return begin;
        }

        template<typename Rng, typename T>
        auto operator()(Rng && rng, T val) const -> CPP_ret(safe_iterator_t<Rng>)( //
            requires OutputRange<Rng, T const &> && WeaklyIncrementable<T>)
        {
            return (*this)(begin(rng), end(rng), detail::move(val));
        }
    };

    RANGES_INLINE_VARIABLE(iota_fn, iota)
    /// @}
} // namespace ranges

#endif
