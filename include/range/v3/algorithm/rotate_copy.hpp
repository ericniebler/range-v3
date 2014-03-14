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
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/algorithm/copy.hpp>

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
            /// \pre rng is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename OutputIterator>
            static OutputIterator invoke(rotater_copier, ForwardIterable && rng,
                range_iterator_t<ForwardIterable> middle, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<ForwardIterable>>());
                detail::copy(ranges::begin(rng), middle,
                    detail::copy(middle, ranges::end(rng), std::move(out)));
            }

            /// \overload
            /// for rng | rotate_copy(middle, out)
            template<typename ForwardIterator, typename OutputIterator>
            static auto invoke(rotater_copier rotate_copy, ForwardIterator middle,
                OutputIterator out) ->
                decltype(rotate_copy.move_bind(std::placeholders::_1, std::move(middle),
                    std::move(out)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterator<ForwardIterator>());
                return rotate_copy.move_bind(std::placeholders::_1, std::move(middle),
                    std::move(out));
            }
        };

        RANGES_CONSTEXPR rotater_copier rotate_copy{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
