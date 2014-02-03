//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_MOVE_HPP
#define RANGES_V3_ALGORITHM_MOVE_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator, typename Sentinel, typename OutputIterator>
            OutputIterator
            move(InputIterator begin, Sentinel end, OutputIterator out)
            {
                for(; begin != end; ++begin, ++out)
                    *out = std::move(*begin);
                return out;
            }
        }

        struct mover : bindable<mover>
        {
            /// \brief template function rotate
            ///
            /// range-based version of the rotate std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename InputIterable, typename OutputIterator>
            static OutputIterator invoke(mover, InputIterable && rng,
                OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                return detail::move(ranges::begin(rng), std::end(rng), std::move(out));
            }

            /// \overload
            /// for rng | move(out)
            template<typename OutputIterator>
            static auto invoke(mover move, OutputIterator out) ->
                decltype(move.move_bind(std::placeholders::_1, std::move(out)))
            {
                return move.move_bind(std::placeholders::_1, std::move(out));
            }
        };

        RANGES_CONSTEXPR mover move{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
