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
#ifndef RANGES_V3_ALGORITHM_REMOVE_HPP
#define RANGES_V3_ALGORITHM_REMOVE_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/algorithm/find.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename Value>
            ForwardIterator
            remove(ForwardIterator begin, Sentinel end, Value const & value)
            {
                begin = detail::find(std::move(begin), end, value);
                if(begin != end)
                    for(auto tmp = ranges::next(begin); tmp != end; ++tmp)
                        if(!(*tmp == value))
                            *begin++ = std::move(*tmp);
                return begin;
            }
        }

        struct remover : bindable<remover>
        {
            /// \brief template function remove
            ///
            /// range-based version of the remove std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename Value>
            static range_iterator_t<ForwardIterable>
            invoke(remover, ForwardIterable && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                return detail::remove(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            /// for rng | remove(val)
            template<typename Value>
            static auto invoke(remover remove, Value && val) ->
                decltype(remove.move_bind(std::placeholders::_1, std::forward<Value>(val)))
            {
                return remove.move_bind(std::placeholders::_1, std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR remover remove {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
