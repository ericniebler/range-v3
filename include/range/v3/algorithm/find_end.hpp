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
#ifndef RANGES_V3_ALGORITHM_FIND_END_HPP
#define RANGES_V3_ALGORITHM_FIND_END_HPP

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
        struct end_finder : bindable<end_finder>
        {
            /// \brief template function \c end_finder::operator()
            ///
            /// range-based version of the \c find_end std algorithm
            ///
            /// \pre \c ForwardRange1 is a model of the ForwardRange concept
            /// \pre \c ForwardRange2 is a model of the ForwardRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange1, typename ForwardRange2>
            static range_iterator_t<ForwardRange1>
            invoke(end_finder, ForwardRange1 && rng1, ForwardRange2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2 const>());
                return std::find_end(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename ForwardRange1, typename ForwardRange2, typename BinaryPredicate>
            static range_iterator_t<ForwardRange1>
            invoke(end_finder, ForwardRange1 && rng1, ForwardRange2 const & rng2,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange1>());
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange2 const>());
                return std::find_end(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2),
                                     ranges::make_invokable(detail::move(pred)));
            }
        };

        RANGES_CONSTEXPR end_finder find_end {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
