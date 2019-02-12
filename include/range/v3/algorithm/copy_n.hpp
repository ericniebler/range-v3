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
#ifndef RANGES_V3_ALGORITHM_COPY_N_HPP
#define RANGES_V3_ALGORITHM_COPY_N_HPP

#include <tuple>
#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using copy_n_result = detail::in_out_result<I, O>;

    struct copy_n_fn
    {
        template<typename I, typename O, typename P = identity>
        auto operator()(I begin, iter_difference_t<I> n, O out) const ->
            CPP_ret(copy_n_result<I, O>)(
                requires InputIterator<I> && WeaklyIncrementable<O> && IndirectlyCopyable<I, O>)
        {
            RANGES_EXPECT(0 <= n);
            auto norig = n;
            auto b = uncounted(begin);
            for(; n != 0; ++b, ++out, --n)
                *out = *b;
            return {recounted(begin, b, norig), out};
        }
    };

    /// \sa `copy_n_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(copy_n_fn, copy_n)

    namespace cpp20
    {
        using ranges::copy_n_result;
        using ranges::copy_n;
    }
    /// @}
} // namespace ranges

#endif // include guard
