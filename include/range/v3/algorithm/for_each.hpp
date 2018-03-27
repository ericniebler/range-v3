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
#include <range/v3/begin_end.hpp>
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
        struct for_each_fn
        {
            template<typename I, typename S, typename F, typename P = ident,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>() &&
                    MoveIndirectInvocable<F, projected<I, P>>())>
            tagged_pair<tag::in(I), tag::fun(F)>
            operator()(I begin, S end, F fun, P proj = P{}) const
            {
                for(; begin != end; ++begin)
                {
                    invoke(fun, invoke(proj, *begin));
                }
                return {detail::move(begin), detail::move(fun)};
            }

            template<typename Rng, typename F, typename P = ident,
                CONCEPT_REQUIRES_(InputRange<Rng>() &&
                    MoveIndirectInvocable<F, projected<iterator_t<Rng>, P>>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::fun(F)>
            operator()(Rng &&rng, F fun, P proj = P{}) const
            {
                return {(*this)(begin(rng), end(rng), ref(fun), detail::move(proj)).in(),
                    detail::move(fun)};
            }
        };

        /// \sa `for_each_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<for_each_fn>, for_each)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
