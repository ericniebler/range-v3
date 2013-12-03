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
#ifndef RANGES_V3_ALGORITHM_SEARCH_N_HPP
#define RANGES_V3_ALGORITHM_SEARCH_N_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct searcher_n : bindable<searcher_n>
        {
            /// \brief template function search
            ///
            /// range-based version of the search std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre Integer is an integral type
            /// \pre Value is a model of the Comparable concept
            /// \pre ForwardRange's value type is a model of the Comparable concept
            /// \pre Object's of ForwardRange's value type can be compared for equality with Objects of type Value
            template<typename ForwardRange, typename Integer, typename Value>
            static range_iterator_t<ForwardRange>
            invoke(searcher_n, ForwardRange && rng, Integer count, Value const & value)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::search_n(ranges::begin(rng), ranges::end(rng), count, value);
            }

            /// \overload
            template<typename ForwardRange, typename Integer, typename Value,
                typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(searcher_n, ForwardRange && rng, Integer count, Value const & value,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_value_t<ForwardRange>,
                                                       Value>());
                return std::search_n(ranges::begin(rng), ranges::end(rng),
                    count, value, ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR searcher_n search_n{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
