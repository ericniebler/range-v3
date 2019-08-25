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

#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using move_backward_result = detail::in_out_result<I, O>;

    RANGES_BEGIN_NIEBLOID(move_backward)

        /// \brief function template \c move_backward
        template<typename I, typename S, typename O>
        auto RANGES_FUN_NIEBLOID(move_backward)(I first, S end_, O out) //
            ->CPP_ret(move_backward_result<I, O>)(                      //
                requires bidirectional_iterator<I> && sentinel_for<S, I> &&
                bidirectional_iterator<O> && indirectly_movable<I, O>)
        {
            I i = ranges::next(first, end_), last = i;
            while(first != i)
                *--out = iter_move(--i);
            return {last, out};
        }

        /// \overload
        template<typename Rng, typename O>
        auto RANGES_FUN_NIEBLOID(move_backward)(Rng && rng, O out)    //
            ->CPP_ret(move_backward_result<safe_iterator_t<Rng>, O>)( //
                requires bidirectional_range<Rng> && bidirectional_iterator<O> &&
                indirectly_movable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }

    RANGES_END_NIEBLOID(move_backward)

    namespace cpp20
    {
        using ranges::move_backward;
        using ranges::move_backward_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
