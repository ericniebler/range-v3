//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_REVERSE_COPY_HPP
#define RANGES_V3_ALGORITHM_REVERSE_COPY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename O>
        using ReverseCopyable = logical_and_t<
            BidirectionalIterator<I>,
            WeaklyIncrementable<O>,
            IndirectlyCopyable<I, O>>;

        struct reverse_copy_fn
        {
            template<typename I, typename S, typename O,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && ReverseCopyable<I, O>())>
            std::pair<I, O> operator()(I begin, S end_, O out) const
            {
                I end = next_to(begin, end_), res = end;
                for (; begin != end; ++out)
                    *out = *--end;
                return {res, out};
            }

            template<typename Rng, typename O,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && ReverseCopyable<I, O>())>
            std::pair<I, O> operator()(Rng &rng, O out) const
            {
                return (*this)(begin(rng), end(rng), std::move(out));
            }
        };

        RANGES_CONSTEXPR reverse_copy_fn reverse_copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
