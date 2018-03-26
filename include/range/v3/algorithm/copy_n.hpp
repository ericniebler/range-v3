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
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

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
                    InputIterator<I>() &&
                    WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>()
                )>
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, difference_type_t<I> n, O out) const
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
        RANGES_INLINE_VARIABLE(with_braced_init_args<copy_n_fn>, copy_n)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
