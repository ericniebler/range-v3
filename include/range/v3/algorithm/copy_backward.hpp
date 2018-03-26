/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_COPY_BACKWARD_HPP
#define RANGES_V3_ALGORITHM_COPY_BACKWARD_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct copy_backward_fn
        {
            template<typename I, typename S, typename O,
                CONCEPT_REQUIRES_(
                    BidirectionalIterator<I>() && Sentinel<S, I>() &&
                    BidirectionalIterator<O>() &&
                    IndirectlyCopyable<I, O>()
                )>
            tagged_pair<tag::in(I), tag::out(O)> operator()(I begin, S end_, O out) const
            {
                I i = ranges::next(begin, end_), end = i;
                while(begin != i)
                    *--out = *--i;
                return {end, out};
            }

            template<typename Rng, typename O,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    BidirectionalRange<Rng>() &&
                    BidirectionalIterator<O>() &&
                    IndirectlyCopyable<I, O>()
                )>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, O out) const
            {
                return (*this)(begin(rng), end(rng), std::move(out));
            }
        };

        /// \sa `copy_backward_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<copy_backward_fn>,
                               copy_backward)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
