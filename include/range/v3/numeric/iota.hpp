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

#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    struct iota_fn
    {
        CPP_template(typename O, typename S, typename T)(
            requires OutputIterator<O, T const &> && Sentinel<S, O> &&
                WeaklyIncrementable<T>)
        O operator()(O begin, S end, T val) const
        {
            for(; begin != end; ++begin, ++val)
                *begin = detail::as_const(val);
            return begin;
        }

        CPP_template(typename Rng, typename T)(
            requires OutputRange<Rng, T const &> && WeaklyIncrementable<T>)
        safe_iterator_t<Rng> operator()(Rng &&rng, T val) const
        {
            return (*this)(begin(rng), end(rng), detail::move(val));
        }
    };

    RANGES_INLINE_VARIABLE(iota_fn, iota)
}

#endif
