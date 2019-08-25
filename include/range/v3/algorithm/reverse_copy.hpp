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
#ifndef RANGES_V3_ALGORITHM_REVERSE_COPY_HPP
#define RANGES_V3_ALGORITHM_REVERSE_COPY_HPP

#include <meta/meta.hpp>

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
    using reverse_copy_result = detail::in_out_result<I, O>;

    RANGES_BEGIN_NIEBLOID(reverse_copy)

        /// \brief function template \c reverse_copy
        template<typename I, typename S, typename O>
        auto RANGES_FUN_NIEBLOID(reverse_copy)(I first, S end_, O out) //
            ->CPP_ret(reverse_copy_result<I, O>)(                      //
                requires bidirectional_iterator<I> && sentinel_for<S, I> &&
                weakly_incrementable<O> && indirectly_copyable<I, O>)
        {
            I last = ranges::next(first, end_), res = last;
            for(; first != last; ++out)
                *out = *--last;
            return {res, out};
        }

        /// \overload
        template<typename Rng, typename O>
        auto RANGES_FUN_NIEBLOID(reverse_copy)(Rng && rng, O out)    //
            ->CPP_ret(reverse_copy_result<safe_iterator_t<Rng>, O>)( //
                requires bidirectional_range<Rng> && weakly_incrementable<O> &&
                indirectly_copyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }

    RANGES_END_NIEBLOID(reverse_copy)

    namespace cpp20
    {
        using ranges::reverse_copy;
        using ranges::reverse_copy_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
