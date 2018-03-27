/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Rostislav Khlebnikov 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_N_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_N_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/algorithm/tagspec.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct for_each_n_fn
        {
            template<typename I, typename F, typename P = ident,
                CONCEPT_REQUIRES_(InputIterator<I>() &&
                    MoveIndirectInvocable<F, projected<I, P>>())>
            I operator()(I begin, difference_type_t<I> n, F fun, P proj = P{}) const
            {
                RANGES_EXPECT(0 <= n);
                auto norig = n;
                auto b = uncounted(begin);
                for(; 0 < n; ++b, --n)
                    invoke(fun, invoke(proj, *b));
                return recounted(begin, b, norig);
            }

            template<typename Rng, typename F, typename P = ident,
                CONCEPT_REQUIRES_(InputRange<Rng>() &&
                    MoveIndirectInvocable<F, projected<iterator_t<Rng>, P>>())>
            safe_iterator_t<Rng>
            operator()(Rng &&rng, range_difference_type_t<Rng> n, F fun, P proj = P{}) const
            {
                if (SizedRange<Rng>())
                    RANGES_EXPECT(n <= distance(rng));

                return (*this)(begin(rng), n, detail::move(fun), detail::move(proj));
            }
        };

        /// \sa `for_each_n_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<for_each_n_fn>, for_each_n)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
