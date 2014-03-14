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
#ifndef RANGES_V3_ALGORITHM_REVERSE_HPP
#define RANGES_V3_ALGORITHM_REVERSE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct reverser : bindable<reverser>, pipeable<reverser>
        {
            /// \brief template function \c reverser::operator()
            ///
            /// range-based version of the \c reverse std algorithm
            ///
            /// \pre \c BidirectionalRange is a model of the BidirectionalRange concept
            template<typename BidirectionalRange>
            static BidirectionalRange invoke(reverser, BidirectionalRange && rng)
            {
                CONCEPT_ASSERT(ranges::Range<BidirectionalRange>());
                CONCEPT_ASSERT(ranges::BidirectionalIterator<range_iterator_t<BidirectionalRange>>());
                std::reverse(ranges::begin(rng), ranges::end(rng));
                return std::forward<BidirectionalRange>(rng);
            }
        };

        RANGES_CONSTEXPR reverser reverse {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
