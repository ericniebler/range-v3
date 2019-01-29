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
#ifndef RANGES_V3_ALGORITHM_MOVE_HPP
#define RANGES_V3_ALGORITHM_MOVE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using move_result = detail::in_out_result<I, O>;

    struct move_fn : aux::move_fn
    {
        using aux::move_fn::operator();

        template<typename I, typename S, typename O>
        auto operator()(I begin, S end, O out) const ->
            CPP_ret(move_result<I, O>)(
                requires InputIterator<I> && Sentinel<S, I> && WeaklyIncrementable<O> &&
                    IndirectlyMovable<I, O>)
        {
            for(; begin != end; ++begin, ++out)
                *out = iter_move(begin);
            return {begin, out};
        }

        template<typename Rng, typename O>
        auto operator()(Rng &&rng, O out) const ->
            CPP_ret(move_result<safe_iterator_t<Rng>, O>)(
                requires InputRange<Rng> && WeaklyIncrementable<O> &&
                    IndirectlyMovable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }
    };

    /// \sa `move_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(move_fn, move)
    /// @}
} // namespace ranges

#endif // include guard
