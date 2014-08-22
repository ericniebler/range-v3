//  Copyright Eric Niebler 2013, 2014
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

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copy_backward_fn
        {
            template<typename I, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    BidirectionalIterator<I>()              &&
                    BidirectionalIterator<O>()              &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            O operator()(I begin, I end, O out, P proj = P{}) const
            {
                auto &&iproj = invokable(proj);
                while(begin != end)
                    *--out = iproj(*--end);
                return out;
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    ConvertibleToBoundedRange<Rng>()                            &&
                    BidirectionalIterator<I>()              &&
                    BidirectionalIterator<O>()              &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            O operator()(Rng &rng, O out, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<copy_backward_fn> copy_backward {};

    } // namespace v3
} // namespace ranges

#endif // include guard
