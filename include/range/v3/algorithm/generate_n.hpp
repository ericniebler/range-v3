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
#ifndef RANGES_V3_ALGORITHM_GENERATE_N_HPP
#define RANGES_V3_ALGORITHM_GENERATE_N_HPP

#include <tuple>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename O, typename F>
    using generate_n_result = detail::out_fun_result<O, F>;

    struct generate_n_fn
    {
        template<typename O, typename F>
        auto operator()(O begin, iter_difference_t<O> n, F fun) const ->
            CPP_ret(generate_n_result<O, F>)(
                requires Invocable<F&> && OutputIterator<O, invoke_result_t<F &>>)
        {
            RANGES_EXPECT(n >= 0);
            auto norig = n;
            auto b = uncounted(begin);
            for(; 0 != n; ++b, --n)
                *b = invoke(fun);
            return {recounted(begin, b, norig), detail::move(fun)};
        }
    };

    /// \sa `generate_n_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(generate_n_fn, generate_n)
    // @}
} // namespace ranges

#endif // include guard
