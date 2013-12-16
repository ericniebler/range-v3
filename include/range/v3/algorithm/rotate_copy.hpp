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
#ifndef RANGES_V3_ALGORITHM_ROTATE_COPY_HPP
#define RANGES_V3_ALGORITHM_ROTATE_COPY_HPP

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
        struct rotater_copier : bindable<rotater_copier>
        {
            /// \brief template function rotate_copy
            ///
            /// range-based version of the rotate_copy std algorithm
            ///
            /// \pre rng is a model of the ForwardRange concept
            template<typename ForwardRange, typename OutputIterator>
            static OutputIterator invoke(rotater_copier, ForwardRange && rng,
                range_iterator_t<ForwardRange> middle, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator, range_value_t<ForwardRange>>());
                return std::rotate_copy(ranges::begin(rng), detail::move(middle),
                    ranges::end(rng), detail::move(out));
            }
        };

        RANGES_CONSTEXPR rotater_copier rotate_copy{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
