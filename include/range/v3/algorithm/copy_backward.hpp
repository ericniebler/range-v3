//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_COPY_BACKWARD_HPP
#define RANGES_V3_ALGORITHM_COPY_BACKWARD_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copy_backward_fn
        {
            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    BidirectionalIterator<I, S>()           &&
                    BidirectionalIterator<O>()              &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<O, I>
            operator()(I begin, S end, O out, P proj = P{}) const
            {
                auto &&iproj = invokable(proj);
                for(; begin != end; ++begin, --out)
                    *out = iproj(*begin);
                return {out, begin};
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>()                         &&
                    BidirectionalIterator<I>()              &&
                    BidirectionalIterator<O>()              &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<O, I>
            operator()(Rng &rng, O out, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(proj));
            }

            template<typename V, typename O, typename P = ident,
                typename I = V const *,
                CONCEPT_REQUIRES_(
                    BidirectionalIterator<O>()              &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<O, I>
            operator()(std::initializer_list<V> rng, O out, P proj = P{}) const
            {
                return (*this)(rng.begin(), rng.end(), std::move(out), std::move(proj));
            }
        };

        RANGES_CONSTEXPR copy_backward_fn copy_backward {};

    } // namespace v3
} // namespace ranges

#endif // include guard
