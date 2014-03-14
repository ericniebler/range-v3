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
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct backward_mover : bindable<backward_mover>
        {
            /// \brief template function rotate
            ///
            /// range-based version of the rotate std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename BidirectionalRange, typename BidirectionalIterator2>
            static BidirectionalIterator2 invoke(backward_mover, BidirectionalRange && rng,
                BidirectionalIterator2 out)
            {
                CONCEPT_ASSERT(ranges::Range<BidirectionalRange>());
                CONCEPT_ASSERT(ranges::BidirectionalIterator<range_iterator_t<BidirectionalRange>>());
                return std::move_backward(ranges::begin(rng), std::end(rng), std::move(out));
            }

            /// \overload
            /// for rng | move_backward(out)
            template<typename BidirectionalIterator2>
            static auto invoke(backward_mover move_backward, BidirectionalIterator2 out) ->
                decltype(move.move_bind(std::placeholders::_1, std::move(out)))
            {
                return move_backward.move_bind(std::placeholders::_1, std::move(out));
            }
        };

        RANGES_CONSTEXPR backward_mover move_backward{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
