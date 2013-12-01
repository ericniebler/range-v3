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
#ifndef RANGES_V3_ALGORITHM_REVERSE_COPY_HPP
#define RANGES_V3_ALGORITHM_REVERSE_COPY_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct reverser_copier : bindable<reverser_copier>
        {
            /// \brief template function reverse_copy
            ///
            /// range-based version of the reverse_copy std algorithm
            ///
            /// \pre BidirectionalRange is a model of the BidirectionalRange concept
            template<typename BidirectionalRange, typename OutputIterator>
            static OutputIterator
            invoke(reverser_copier, BidirectionalRange && rng, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                return std::reverse_copy(ranges::begin(rng), ranges::end(rng), out);
            }

            /// \overload
            /// for rng | reverse_copy(out)
            template<typename OutputIterator>
            static auto invoke(reverser_copier reverse_copy, OutputIterator out)
                -> decltype(reverse_copy(std::placeholders::_1, detail::move(out)))
            {
                return reverse_copy(std::placeholders::_1, detail::move(out));
            }
        };

        RANGES_CONSTEXPR reverser_copier reverse_copy{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
