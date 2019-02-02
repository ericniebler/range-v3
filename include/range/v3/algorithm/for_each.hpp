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
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/functional/reference_wrapper.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename F>
    using for_each_result = detail::in_fun_result<I, F>;

    struct for_each_fn
    {
        CPP_template(typename I, typename S, typename F, typename P = identity)(
            requires InputIterator<I> && Sentinel<S, I> &&
                IndirectUnaryInvocable<F, projected<I, P>>)
        for_each_result<I, F> operator()(I begin, S end, F fun, P proj = P{}) const
        {
            for(; begin != end; ++begin)
            {
                invoke(fun, invoke(proj, *begin));
            }
            return {detail::move(begin), detail::move(fun)};
        }

        CPP_template(typename Rng, typename F, typename P = identity)(
            requires InputRange<Rng> &&
                IndirectUnaryInvocable<F, projected<iterator_t<Rng>, P>>)
        for_each_result<safe_iterator_t<Rng>, F> operator()(Rng &&rng, F fun, P proj = P{}) const
        {
            return {(*this)(begin(rng), end(rng), ref(fun), detail::move(proj)).in,
                detail::move(fun)};
        }
    };

    /// \sa `for_each_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(for_each_fn, for_each)
    /// @}
} // namespace ranges

#endif // include guard
