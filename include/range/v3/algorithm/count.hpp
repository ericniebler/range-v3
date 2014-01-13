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
#ifndef RANGES_V3_ALGORITHM_COUNT_HPP
#define RANGES_V3_ALGORITHM_COUNT_HPP

#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator, typename Sentinel, typename Value>
            iterator_difference_t<InputIterator>
            count(InputIterator begin, Sentinel end, Value const & val)
            {
                iterator_difference_t<InputIterator> n = 0;
                for(; begin != end; ++begin)
                    if(*begin == val)
                        ++n;
                return n;
            }
        }

        struct counter : bindable<counter>
        {
            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c InputIterable is a model of the InputIterable concept
            template<typename InputIterable, typename Value>
            static range_difference_t<InputIterable>
            invoke(counter, InputIterable && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                return detail::count(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c InputIterable is a model of the InputIterable concept
            template<typename Value>
            static auto invoke(counter count, Value && val) ->
                decltype(count.move_bind(std::placeholders::_1, std::forward<Value>(val)))
            {
                return count.move_bind(std::placeholders::_1, std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR counter count {};
    } // inline namespace v3

} // namespace ranges

#endif // include guard
