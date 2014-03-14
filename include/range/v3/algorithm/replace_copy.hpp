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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel,
                     typename OutputIterator, typename Value>
            OutputIterator
            replace_copy(ForwardIterator begin, Sentinel end, OutputIterator out,
                Value const& old_value, Value const& new_value)
            {
                for(; begin != end; ++begin, ++out)
                    *out = (*begin == old_value) ? new_value : *begin;
                return out;
            }
        }

        struct replacer_copier : bindable<replacer_copier>
        {
            /// \brief template function replace_copy
            ///
            /// range-based version of the replace_copy std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            template<typename ForwardIterable, typename OutputIterator, typename Value>
            static OutputIterator
            invoke(replacer_copier, ForwardIterable && rng, OutputIterator out,
                   Value const & old_value, Value const & new_value)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<ForwardIterable>>());
                return detail::replace_copy(ranges::begin(rng), ranges::end(rng),
                    std::move(out), old_value, new_value);
            }

            // BUGBUG should "rng | replace_copy(this, that)" be lazy?

            /// \overload
            /// for rng | replace_copy(out, that, that)
            template<typename OutputIterator, typename Value1, typename Value2>
            static auto invoke(replacer_copier replace_copy, OutputIterator out, Value1 && old_value,
                Value2 && new_value) ->
                decltype(replace_copy.move_bind(std::placeholders::_1, std::move(out),
                    std::forward<Value1>(old_value), std::forward<Value2>(new_value)))
            {
                CONCEPT_ASSERT(ranges::Iterator<OutputIterator>());
                return replace_copy.move_bind(std::placeholders::_1, std::move(out),
                    std::forward<Value1>(old_value), std::forward<Value2>(new_value));
            }
        };

        RANGES_CONSTEXPR replacer_copier replacer_copy{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
