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
#ifndef RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP

#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct adjacent_finder
        {
            /// \brief function template \c adjacent_finder::operator()
            ///
            /// range-based version of the \c adjacent_find std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange>
            range_iterator_t<ForwardRange>
            operator()(ForwardRange && rng) const
            {
                static_assert(ranges::ForwardRange<ForwardRange>(),
                    "Expecting model of ForwardRange");
                return std::adjacent_find(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardRange, typename BinaryPredicate>
            range_iterator_t<ForwardRange>
            operator()(ForwardRange && rng, BinaryPredicate pred) const
            {
                static_assert(ranges::ForwardRange<ForwardRange>(),
                    "Expecting model of ForwardRange");
                static_assert(
                    ranges::BinaryPredicate<BinaryPredicate,
                                            range_reference_t<ForwardRange>,
                                            range_reference_t<ForwardRange>>(),
                    "Expecting model of BinaryPredicate");
                return std::adjacent_find(ranges::begin(rng), ranges::end(rng), detail::move(pred));
            }
        };
        
        constexpr bindable<adjacent_finder> adjacent_find {};
    } // namespace v3
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
