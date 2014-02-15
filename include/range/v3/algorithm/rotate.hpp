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
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // With credit to Howard Hinnant
            template<typename ForwardIterator, typename Sentinel>
            ForwardIterator
            rotate(ForwardIterator begin, ForwardIterator middle, Sentinel end,
                concepts::ForwardIterable)
            {
                if(begin == middle)
                    return end;
                if(middle == end)
                    return begin;
                for(auto i = middle;;)
                {
                    ranges::swap(*begin, *i);
                    ++begin;
                    if(++i == end)
                        break;
                    if(begin == middle)
                        middle = i;
                }
                auto ret = begin;
                if(begin != middle)
                {
                    for(auto i = middle;;)
                    {
                        ranges::swap(*begin, *i);
                        ++begin;
                        if(++i == end)
                        {
                            if(begin == middle)
                                break;
                            i = middle;
                        }
                        else if (begin == middle)
                            middle = i;
                    }
                }
                return ret;
            }

            template<typename ForwardIterator>
            ForwardIterator
            rotate(ForwardIterator begin, ForwardIterator middle, ForwardIterator end,
                concepts::ForwardRange)
            {
                return std::rotate(std::move(begin), std::move(middle), std::move(end));
            }
        }

        struct rotater : bindable<rotater>
        {
            /// \brief template function rotate
            ///
            /// range-based version of the rotate std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename ForwardIterable>
            static range_iterator_t<ForwardIterable>
            invoke(rotater, ForwardIterable && rng, range_iterator_t<ForwardIterable> middle)
            {
                CONCEPT_ASSERT(ranges::FiniteForwardIterable<ForwardIterable>());
                return detail::rotate(ranges::begin(rng), std::move(middle), ranges::end(rng),
                    range_concept_t<ForwardIterable>{});
            }

            /// \overload
            /// for rng | rotate(middle)
            template<typename ForwardIterator>
            static auto invoke(rotater rotate, ForwardIterator middle) ->
                decltype(rotate.move_bind(std::placeholders::_1, std::move(middle)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterator<ForwardIterator>());
                return rotate.move_bind(std::placeholders::_1, std::move(middle));
            }
        };

        RANGES_CONSTEXPR rotater rotate{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
