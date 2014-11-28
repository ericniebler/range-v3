// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copy_backward_fn
        {
            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    BidirectionalIterator<I>() && IteratorRange<I, S>() &&
                    BidirectionalIterator<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O> operator()(I begin, S end_, O out, P proj = P{}) const
            {
                auto &&iproj = invokable(proj);
                I i = next_to(begin, end_), end = i;
                while(begin != i)
                    *--out = iproj(*--i);
                return {end, out};
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    BidirectionalIterable<Rng &>() &&
                    BidirectionalIterator<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O> operator()(Rng &rng, O out, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(proj));
            }
        };

        constexpr copy_backward_fn copy_backward {};

    } // namespace v3
} // namespace ranges

#endif // include guard
