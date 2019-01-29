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
#ifndef RANGES_V3_ALGORITHM_FILL_HPP
#define RANGES_V3_ALGORITHM_FILL_HPP

#include <range/v3/range_fwd.hpp>
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
    struct fill_fn
    {
        template<typename O, typename S, typename V>
        auto operator()(O begin, S end, V const & val) const ->
            CPP_ret(O)(
                requires OutputIterator<O, V const &> && Sentinel<S, O>)
        {
            for(; begin != end; ++begin)
                *begin = val;
            return begin;
        }

        template<typename Rng, typename V>
        auto operator()(Rng &&rng, V const & val) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires OutputRange<Rng, V const &>)
        {
            return (*this)(begin(rng), end(rng), val);
        }
    };

    /// \sa `fill_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(fill_fn, fill)
    /// @}
} // namespace ranges

#endif // include guard
