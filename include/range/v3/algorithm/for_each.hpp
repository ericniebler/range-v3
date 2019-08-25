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
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <functional>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/functional/reference_wrapper.hpp>
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
    template<typename I, typename F>
    using for_each_result = detail::in_fun_result<I, F>;

    RANGES_BEGIN_NIEBLOID(for_each)

        /// \brief function template \c for_each
        template<typename I, typename S, typename F, typename P = identity>
        auto RANGES_FUN_NIEBLOID(for_each)(I first, S last, F fun, P proj = P{})
            ->CPP_ret(for_each_result<I, F>)( //
                requires input_iterator<I> && sentinel_for<S, I> &&
                indirectly_unary_invocable<F, projected<I, P>>)
        {
            for(; first != last; ++first)
            {
                invoke(fun, invoke(proj, *first));
            }
            return {detail::move(first), detail::move(fun)};
        }

        /// \overload
        template<typename Rng, typename F, typename P = identity>
        auto RANGES_FUN_NIEBLOID(for_each)(Rng && rng, F fun, P proj = P{})
            ->CPP_ret(for_each_result<safe_iterator_t<Rng>, F>)( //
                requires input_range<Rng> &&
                indirectly_unary_invocable<F, projected<iterator_t<Rng>, P>>)
        {
            return {(*this)(begin(rng), end(rng), ref(fun), detail::move(proj)).in,
                    detail::move(fun)};
        }

    RANGES_END_NIEBLOID(for_each)

    namespace cpp20
    {
        using ranges::for_each;
        using ranges::for_each_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
