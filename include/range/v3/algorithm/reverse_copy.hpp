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
    using reverse_copy_result = detail::in_out_result<I, O>;

    struct reverse_copy_fn
    {
        template<typename I, typename S, typename O>
        auto operator()(I begin, S end_, O out) const ->
            CPP_ret(reverse_copy_result<I, O>)(
                requires BidirectionalIterator<I> && Sentinel<S, I> &&
                    WeaklyIncrementable<O> &&
                    IndirectlyCopyable<I, O>)
        {
            I end = ranges::next(begin, end_), res = end;
            for(; begin != end; ++out)
                *out = *--end;
            return {res, out};
        }

        template<typename Rng, typename O>
        auto operator()(Rng &&rng, O out) const ->
            CPP_ret(reverse_copy_result<safe_iterator_t<Rng>, O>)(
                requires BidirectionalRange<Rng> && WeaklyIncrementable<O> &&
                    IndirectlyCopyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }
    };

    /// \sa `reverse_copy_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(reverse_copy_fn, reverse_copy)

    namespace cpp20
    {
        using ranges::reverse_copy_result;
        using ranges::reverse_copy;
    }
    /// @}
} // namespace ranges

#endif // include guard
