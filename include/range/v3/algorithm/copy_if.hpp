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
#ifndef RANGES_V3_ALGORITHM_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_COPY_IF_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        template<typename I, typename O>
        using copy_if_result = detail::in_out_result<I, O>;

        struct copy_if_fn
        {
            template<typename I, typename S, typename O, typename F, typename P = identity>
            auto operator()(I begin, S end, O out, F pred, P proj = P{}) const ->
                CPP_ret(copy_if_result<I, O>)(
                    requires InputIterator<I> && Sentinel<S, I> && WeaklyIncrementable<O> &&
                        IndirectPredicate<F, projected<I, P>> && IndirectlyCopyable<I, O>)
            {
                for(; begin != end; ++begin)
                {
                    auto &&x = *begin;
                    if(invoke(pred, invoke(proj, x)))
                    {
                        *out = (decltype(x) &&) x;
                        ++out;
                    }
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename F, typename P = identity>
            auto operator()(Rng &&rng, O out, F pred, P proj = P{}) const ->
                CPP_ret(copy_if_result<safe_iterator_t<Rng>, O>)(
                    requires InputRange<Rng> && WeaklyIncrementable<O> &&
                        IndirectPredicate<F, projected<iterator_t<Rng>, P>> &&
                        IndirectlyCopyable<iterator_t<Rng>, O>)
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `copy_if_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<copy_if_fn>, copy_if)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
