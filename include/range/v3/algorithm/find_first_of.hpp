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
#ifndef RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP
#define RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP

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
        struct first_of_finder : bindable<first_of_finder>
        {
            /// \brief template function \c first_of_finder::operator()
            ///
            /// range-based version of the \c find_first_of std algorithm
            ///
            /// \pre \c InputRange1 is a model of the InputRange concept
            /// \pre \c ForwardRange2 is a model of the ForwardRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename ForwardRange2,
                CONCEPT_REQUIRES(ranges::Range<InputRange1>() &&
                                 ranges::Range<ForwardRange2 const>())>
            static range_iterator_t<InputRange1>
            invoke(first_of_finder, InputRange1 && rng1, ForwardRange2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2 const>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<InputRange1>,
                                                          range_reference_t<ForwardRange2 const>>());
                return std::find_first_of(ranges::begin(rng1), ranges::end(rng1),
                                          ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename ForwardRange2, typename BinaryPredicate>
            static range_iterator_t<InputRange1>
            invoke(first_of_finder, InputRange1 && rng1, ForwardRange2 const & rng2,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2 const>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<BinaryPredicate,
                                                       range_reference_t<InputRange1>,
                                                       range_reference_t<ForwardRange2 const>>());
                return std::find_first_of(ranges::begin(rng1), ranges::end(rng1),
                                          ranges::begin(rng2), ranges::end(rng2),
                                          ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | find_first_of(rng2)
            template<typename ForwardRange2>
            static auto
            invoke(first_of_finder find_first_of, ForwardRange2 && rng2) ->
                decltype(find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardRange2>(rng2)))
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2>());
                return find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardRange2>(rng2));
            }

            /// \overload
            /// for rng | find_first_of(rng2, pred)
            template<typename ForwardRange2, typename BinaryPredicate,
                CONCEPT_REQUIRES(ranges::Range<ForwardRange2>() &&
                                !ranges::Range<BinaryPredicate>())>
            static auto
            invoke(first_of_finder find_first_of, ForwardRange2 && rng2, BinaryPredicate pred) ->
                decltype(find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardRange2>(rng2), std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2>());
                return find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardRange2>(rng2), std::move(pred));
            }
        };

        RANGES_CONSTEXPR first_of_finder find_first_of {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
