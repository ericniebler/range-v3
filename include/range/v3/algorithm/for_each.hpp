/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
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
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct for_each_fn
        {
            template<typename I, typename S, typename F, typename P = ident,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>() &&
                    IndirectCallable<F, projected<I, P>>())>
            I operator()(I begin, S end, F fun_, P proj_ = P{}) const
            {
                auto &&fun = as_function(fun_);
                auto &&proj = as_function(proj_);
                for(; begin != end; ++begin)
                {
                    fun(proj(*begin));
                }
                return begin;
            }

            template<typename Rng, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(InputRange<Rng>() && IndirectCallable<F, projected<I, P>>())>
            range_safe_iterator_t<Rng> operator()(Rng &&rng, F fun, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(fun), std::move(proj));
            }
        };

        /// \sa `for_each_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<for_each_fn>, for_each)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
