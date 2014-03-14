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
#include <range/v3/iterator_range.hpp>
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
            rotate(ForwardIterator begin, ForwardIterator middle, Sentinel end)
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
            rotate(ForwardIterator begin, ForwardIterator middle, ForwardIterator end)
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
            /// \pre ForwardRange models ForwardRange
            template<typename ForwardRange, typename Sentinel>
            static range_iterator_t<ForwardRange>
            invoke(rotater, ForwardRange && rng, Sentinel to)
            {
                using ForwardIterable = iterator_range<range_iterator_t<ForwardRange>,
                                                       Sentinel>;
                CONCEPT_ASSERT(ranges::Range<ForwardRange>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardRange>>());
                return detail::rotate(ranges::begin(rng), ranges::end(rng), std::move(to));
            }

            /// \overload
            /// for rng | rotate(to)
            template<typename Sentinel>
            static auto invoke(rotater rotate, Sentinel to) ->
                decltype(rotate.move_bind(std::placeholders::_1, std::move(to)))
            {
                return rotate.move_bind(std::placeholders::_1, std::move(to));
            }
        };

        RANGES_CONSTEXPR rotater rotate{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
