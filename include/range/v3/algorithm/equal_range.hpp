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
#ifndef RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP
#define RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct equal_ranger
        {
            /// \brief template function \c equal_ranger::operator()
            ///
            /// range-based version of the \c equal_range std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            /// \pre \c SortPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange, typename Value>
            iterator_range<range_iterator_t<ForwardRange>>
            operator()(ForwardRange && rng, Value const & val) const
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::equal_range(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            template<typename ForwardRange, typename Value, typename SortPredicate>
            iterator_range<range_iterator_t<ForwardRange>>
            operator()(ForwardRange && rng, Value const & val, SortPredicate pred) const
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::equal_range(ranges::begin(rng), ranges::end(rng), val, std::move(pred));
            }
        };

        constexpr bindable<equal_ranger> equal_range {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
