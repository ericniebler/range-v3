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
#ifndef RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel,
                     typename BinaryPredicate = ranges::less>
            ForwardIterator
            min_element(ForwardIterator begin, Sentinel end,
                        BinaryPredicate pred = BinaryPredicate{})
            {
                if(begin != end)
                    for(auto tmp = ranges::next(begin); tmp != end; ++tmp)
                        if(pred(*tmp, *begin))
                            begin = tmp;
                return begin;
            }
        }

        struct min_element_finder : bindable<min_element_finder>,
                                    pipeable<min_element_finder>
        {
            /// \brief template function min_element
            ///
            /// range-based version of the min_element std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterable,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>())>
            static range_iterator_t<ForwardIterable>
            invoke(min_element_finder, ForwardIterable && rng)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<ForwardIterable>>());
                return detail::min_element(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardIterable, typename BinaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(min_element_finder, ForwardIterable && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable>,
                                                       range_reference_t<ForwardIterable>>());
                return detail::min_element(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | min_element(pred)
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES_(!ranges::Iterable<BinaryPredicate>())>
            static auto
            invoke(min_element_finder min_element, BinaryPredicate pred) ->
                decltype(min_element.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return min_element.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR min_element_finder min_element {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
