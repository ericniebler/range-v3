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
#ifndef RANGES_V3_ALGORITHM_UNIQUE_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        // TODO: make a unique view, a-la filter

        struct uniquer : bindable<uniquer>
        {
            /// \brief template function unique
            ///
            /// range-based version of the unique std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename ForwardRange>
            static range_iterator_t<ForwardRange>
            invoke(uniquer, ForwardRange && rng)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::unique(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardRange, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(uniquer, ForwardRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<BinaryPredicate,
                                                       range_reference_t<ForwardRange>,
                                                       range_reference_t<ForwardRange>>());
                return std::unique(ranges::begin(rng), ranges::end(rng), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR uniquer unique {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
