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
#ifndef RANGES_V3_ALGORITHM_REVERSE_HPP
#define RANGES_V3_ALGORITHM_REVERSE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct reverse_fn
        {
        private:
            template<typename I>
            static void impl(I begin, I end, concepts::BidirectionalIterator*)
            {
                while(begin != end)
                {
                    if(begin == --end)
                        break;
                    ranges::iter_swap(begin, end);
                    ++begin;
                }
            }

            template<typename I>
            static void impl(I begin, I end, concepts::RandomAccessIterator*)
            {
                if(begin != end)
                    for(; begin < --end; ++begin)
                        ranges::iter_swap(begin, end);
            }

        public:
            template<typename I, typename S,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sentinel<S, I>() && Permutable<I>())>
            I operator()(I begin, S end_) const
            {
                I end = ranges::next(begin, end_);
                reverse_fn::impl(begin, end, iterator_concept<I>{});
                return end;
            }

            template<typename Rng, typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && Permutable<I>())>
            safe_iterator_t<Rng> operator()(Rng &&rng) const
            {
                return (*this)(begin(rng), end(rng));
            }
        };

        /// \sa `reverse_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<reverse_fn>, reverse)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
