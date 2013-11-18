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

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct backward_copier
        {
            /// \brief template function \c backward_copier::operator()
            ///
            /// range-based version of the \c copy_backwards std algorithm
            ///
            /// \pre \c BidirectionalRange is a model of the BidirectionalRange concept
            /// \pre \c BidirectionalIterator is a model of the BidirectionalIterator concept
            /// \pre \c BidirectionalIterator is a model of the Iterator concept
            template<typename BidirectionalRange, typename BidirectionalIterator>
            BidirectionalIterator operator()(BidirectionalRange && rng,
                                             BidirectionalIterator out) const
            {
                static_assert(ranges::BidirectionalRange<BidirectionalRange>(),
                    "Expecting model of BidirectionalRange");
                return std::copy_backward(ranges::begin(rng), ranges::end(rng), std::move(out));
            }
        };

        constexpr bindable<backward_copier> copy_backward {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
