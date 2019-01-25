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
#ifndef RANGES_V3_ALGORITHM_MOVE_BACKWARD_HPP
#define RANGES_V3_ALGORITHM_MOVE_BACKWARD_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using move_backward_result = detail::in_out_result<I, O>;

    struct move_backward_fn
    {
        template<typename I, typename S, typename O>
        auto operator()(I begin, S end_, O out) const ->
            CPP_ret(move_backward_result<I, O>)(
                requires BidirectionalIterator<I> && Sentinel<S, I> && BidirectionalIterator<O> && IndirectlyMovable<I, O>)
        {
            I i = ranges::next(begin, end_), end = i;
            while(begin != i)
                *--out = iter_move(--i);
            return {end, out};
        }

        template<typename Rng, typename O>
        auto operator()(Rng &&rng, O out) const ->
            CPP_ret(move_backward_result<safe_iterator_t<Rng>, O>)(
                requires BidirectionalRange<Rng> && BidirectionalIterator<O> &&
                    IndirectlyMovable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }
    };

    /// \sa `move_backward_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(move_backward_fn, move_backward)
    /// @}
} // namespace ranges

#endif // include guard
