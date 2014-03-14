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
#ifndef RANGES_V3_ALGORITHM_REPLACE_HPP
#define RANGES_V3_ALGORITHM_REPLACE_HPP

#include <utility>
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
            void
            replace(ForwardIterator begin, Sentinel end,
                Value const& old_value, Value const& new_value)
            {
                for(; begin != end; ++begin)
                    if(*begin == old_value)
                        *begin = new_value;
            }
        }

        struct replacer : bindable<replacer>
        {
            /// \brief template function replace
            ///
            /// range-based version of the replace std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename Value>
            static ForwardIterable
            invoke(replacer, ForwardIterable && rng, Value const & old_value,
                Value const & new_value)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                detail::replace(ranges::begin(rng), ranges::end(rng), old_value, new_value);
                return std::forward<ForwardIterable>(rng);
            }

            /// \overload
            /// for rng | replace(that, that)
            template<typename Value1, typename Value2>
            static auto invoke(replacer replace, Value1 && old_value, Value2 && new_value) ->
                decltype(replace.move_bind(std::placeholders::_1, std::forward<Value1>(old_value),
                    std::forward<Value2>(new_value)))
            {
                return replace.move_bind(std::placeholders::_1, std::forward<Value1>(old_value),
                    std::forward<Value2>(new_value));
            }
        };

        RANGES_CONSTEXPR replacer replace{};

    } // inline namespace v3

} // namespace ranges;

#endif // include guard
