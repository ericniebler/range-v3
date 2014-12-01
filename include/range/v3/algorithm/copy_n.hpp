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
#ifndef RANGES_V3_ALGORITHM_COPY_N_HPP
#define RANGES_V3_ALGORITHM_COPY_N_HPP

#include <tuple>
#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct copy_n_fn
        {
            template<typename I, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    WeakInputIterator<I>() &&
                    WeaklyIncrementable<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(I begin, iterator_difference_t<I> n, O out, P proj_ = P{}) const
            {
                RANGES_ASSERT(0 <= n);
                auto &&proj = invokable(proj_);
                auto norig = n;
                auto b = uncounted(begin);
                for(; n != 0; ++b, ++out, --n)
                    *out = proj(*b);
                return {recounted(begin, b, norig), out};
            }
        };

        /// \sa `copy_n_fn`
        /// \ingroup group-algorithms
        constexpr copy_n_fn copy_n{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
