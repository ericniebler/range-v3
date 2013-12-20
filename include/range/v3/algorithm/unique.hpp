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
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct uniquer : bindable<uniquer>, pipeable<uniquer>
        {
            /// \brief template function unique
            ///
            /// range-based version of the unique std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename ForwardRange,
                CONCEPT_REQUIRES(ranges::Range<ForwardRange>())>
            static range_iterator_t<ForwardRange>
            invoke(uniquer, ForwardRange && rng)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<ForwardRange>>());
                CONCEPT_ASSERT(ranges::MoveAssignable<range_reference_t<ForwardRange>>());
                return std::unique(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardRange, typename BinaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(uniquer, ForwardRange && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardRange>,
                                                       range_reference_t<ForwardRange>>());
                CONCEPT_ASSERT(ranges::MoveAssignable<range_reference_t<ForwardRange>>());
                return std::unique(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)));
            }

            /// \overload
            /// for rng | unique(pred)
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES(!ranges::Range<BinaryPredicate>())>
            static auto
            invoke(uniquer unique, BinaryPredicate pred)
                -> decltype(unique(std::placeholders::_1, detail::move(pred)))
            {
                return unique(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR uniquer unique {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
