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
#ifndef RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MAX_ELEMENT_HPP

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
            max_element(ForwardIterator begin, Sentinel end,
                        BinaryPredicate pred = BinaryPredicate{})
            {
                if(begin != end)
                    for(auto tmp = ranges::next(begin); tmp != end; ++tmp)
                        if(pred(*begin, *tmp))
                            begin = tmp;
                return begin;
            }
        }

        struct max_element_finder : bindable<max_element_finder>,
                                    pipeable<max_element_finder>
        {
            /// \brief template function max_element
            ///
            /// range-based version of the max_element std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterable,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>())>
            static range_iterator_t<ForwardIterable>
            invoke(max_element_finder, ForwardIterable && rng)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::LessThanComparable<range_reference_t<ForwardIterable>>());
                return std::max_element(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardIterable, typename BinaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(max_element_finder, ForwardIterable && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::Predicate<invokable_t<BinaryPredicate>,
                                                 range_reference_t<ForwardIterable>,
                                                 range_reference_t<ForwardIterable>>());
                return std::max_element(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | max_element(pred)
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES_(!ranges::Iterable<BinaryPredicate>())>
            static auto
            invoke(max_element_finder max_element, BinaryPredicate pred) ->
                decltype(max_element.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return max_element.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR max_element_finder max_element {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
