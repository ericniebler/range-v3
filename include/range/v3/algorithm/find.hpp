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
#ifndef RANGES_V3_ALGORITHM_FIND_HPP
#define RANGES_V3_ALGORITHM_FIND_HPP

#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename Value>
            ForwardIterator find(ForwardIterator begin, Sentinel end, Value const & val)
            {
                for(; begin != end; ++begin)
                    if(*begin == val)
                        break;
                return begin;
            }
        }

        struct finder : bindable<finder>
        {
            /// \brief template function \c finder::operator()
            ///
            /// range-based version of the \c find std algorithm
            ///
            /// \pre \c InputIterable is a model of the InputIterable concept
            template<typename InputIterable, typename Value>
            static range_iterator_t<InputIterable>
            invoke(finder, InputIterable && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                return detail::find(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            /// rng | find(val)
            template<typename Value>
            static auto invoke(finder find, Value && val)
                -> decltype(find.move_bind(std::placeholders::_1, std::forward<Value>(val)))
            {
                return find.move_bind(std::placeholders::_1, std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR finder find {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
