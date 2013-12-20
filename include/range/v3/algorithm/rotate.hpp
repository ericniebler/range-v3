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
#ifndef RANGES_V3_ALGORITHM_ROTATE_HPP
#define RANGES_V3_ALGORITHM_ROTATE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct rotater : bindable<rotater>
        {
            /// \brief template function rotate
            ///
            /// range-based version of the rotate std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename ForwardRange>
            static ForwardRange invoke(rotater, ForwardRange && rng,
                range_iterator_t<ForwardRange> middle)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                std::rotate(ranges::begin(rng), std::move(middle), ranges::end(rng));
                return std::forward<ForwardRange>(rng);
            }

            /// \overload
            /// for rng | rotate(middle)
            template<typename ForwardIterator>
            static auto invoke(rotater rotate, ForwardIterator middle) ->
                decltype(rotate(std::placeholders::_1, std::move(middle)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterator<ForwardIterator>());
                return rotate(std::placeholders::_1, std::move(middle));
            }
        };

        RANGES_CONSTEXPR rotater rotate{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
