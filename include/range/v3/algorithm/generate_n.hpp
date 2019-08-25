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

#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/invoke.hpp>
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
    template<typename O, typename F>
    using generate_n_result = detail::out_fun_result<O, F>;

    RANGES_BEGIN_NIEBLOID(generate_n)

        /// \brief function template \c generate_n
        template<typename O, typename F>
        auto RANGES_FUN_NIEBLOID(generate_n)(O first, iter_difference_t<O> n, F fun)
            ->CPP_ret(generate_n_result<O, F>)( //
                requires invocable<F &> && output_iterator<O, invoke_result_t<F &>>)
        {
            RANGES_EXPECT(n >= 0);
            auto norig = n;
            auto b = uncounted(first);
            for(; 0 != n; ++b, --n)
                *b = invoke(fun);
            return {recounted(first, b, norig), detail::move(fun)};
        }

    RANGES_END_NIEBLOID(generate_n)

    namespace cpp20
    {
        using ranges::generate_n;
        using ranges::generate_n_result;
    } // namespace cpp20
    // @}
} // namespace ranges

#endif // include guard
