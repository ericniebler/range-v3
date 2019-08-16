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
#ifndef RANGES_V3_ALGORITHM_FILL_N_HPP
#define RANGES_V3_ALGORITHM_FILL_N_HPP

#include <utility>

#include <range/v3/range_fwd.hpp>

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
    struct fill_n_fn
    {
        template<typename O, typename V>
        auto operator()(O begin, iter_difference_t<O> n, V const & val) const
            -> CPP_ret(O)( //
                requires output_iterator<O, V const &>)
        {
            RANGES_EXPECT(n >= 0);
            auto norig = n;
            auto b = uncounted(begin);
            for(; n != 0; ++b, --n)
                *b = val;
            return recounted(begin, b, norig);
        }
    };

    /// \sa `fill_n_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(fill_n_fn, fill_n)

    namespace cpp20
    {
        using ranges::fill_n;
    }
    /// @}
} // namespace ranges

#endif // include guard
