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

#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel,
                typename BinaryPredicate = ranges::equal_to>
            ForwardIterator
            adjacent_find(ForwardIterator begin, Sentinel end,
                BinaryPredicate pred = BinaryPredicate{})
            {
                if(begin == end)
                    return begin;
                auto next = begin;
                for(; ++next != end; begin = next)
                    if(pred(*begin, *next))
                        return begin;
                return next;
            }
        }

        struct adjacent_finder : bindable<adjacent_finder>,
                                 pipeable<adjacent_finder>
        {
            /// \brief function template \c adjacent_finder::operator()
            ///
            /// range-based version of the \c adjacent_find std algorithm
            ///
            /// \pre \c ForwardIterable is a model of the ForwardIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterable>
            static range_iterator_t<ForwardIterable>
            invoke(adjacent_finder, ForwardIterable && rng)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<ForwardIterable>>());
                return detail::adjacent_find(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardIterable, typename BinaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(adjacent_finder, ForwardIterable && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable>,
                                                       range_reference_t<ForwardIterable>>());
                return detail::adjacent_find(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR adjacent_finder adjacent_find {};

    } // namespace v3
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
