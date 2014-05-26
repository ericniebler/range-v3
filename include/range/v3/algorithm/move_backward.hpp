//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_MOVE_BACKWARD_HPP
#define RANGES_V3_ALGORITHM_MOVE_BACKWARD_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct move_backward_fn
        {
            template<typename I, typename O, typename P = ident,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>() && BidirectionalIterator<O>() &&
                    IndirectlyProjectedMovable<I, P, O>())>
            O operator()(I begin, I end, O out, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                while(begin != end)
                    *--out = std::move(proj(*--end));
                return out;
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(BidirectionalIterable<Rng>() && BidirectionalIterator<O>() &&
                    IndirectlyProjectedMovable<I, P, O>())>
            O operator()(Rng &rng, O out, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(proj));
            }
        };

        RANGES_CONSTEXPR move_backward_fn move_backward{};

    } // namespace v3
} // namespace ranges

#endif // include guard
