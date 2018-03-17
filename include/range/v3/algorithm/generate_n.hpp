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
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
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
        struct generate_n_fn
        {
            template<typename O, typename F,
                CONCEPT_REQUIRES_(Invocable<F &>() &&
                    OutputIterator<O, invoke_result_t<F &>>())>
            tagged_pair<tag::out(O), tag::fun(F)>
            operator()(O begin, difference_type_t<O> n, F fun) const
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
    } // namespace v3
} // namespace ranges

#endif // include guard
