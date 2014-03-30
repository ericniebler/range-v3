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
#ifndef RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator, typename Sentinel,
                     typename OutputIterator, typename BinaryPredicate>
            OutputIterator
            unique_copy(InputIterator begin, Sentinel end, OutputIterator out, BinaryPredicate pred,
                        concepts::InputIterator, concepts::OutputIterator)
            {
                if(begin != end)
                {
                    auto value{*begin};
                    *out = value;
                    ++out;
                    while(++begin != end)
                    {
                        if(!pred(value, *begin))
                        {
                            value = *begin;
                            *out = value;
                            ++out;
                        }
                    }
                }
                return out;
            }

            template<typename ForwardIterator, typename Sentinel,
                     typename OutputIterator, typename BinaryPredicate>
            OutputIterator
            unique_copy(ForwardIterator begin, Sentinel end, OutputIterator out, BinaryPredicate pred,
                        concepts::ForwardIterator, concepts::OutputIterator)
            {
                if(begin != end)
                {
                    auto tmp = begin;
                    *out = *tmp;
                    ++out;
                    while(++begin != end)
                    {
                        if(!pred(*tmp, *begin))
                        {
                            *out = *begin;
                            ++out;
                            tmp = begin;
                        }
                    }
                }
                return out;
            }

            template<typename InputIterator, typename ForwardIterator, typename Sentinel,
                     typename BinaryPredicate>
            ForwardIterator
            unique_copy(InputIterator begin, Sentinel end, ForwardIterator out, BinaryPredicate pred,
                        concepts::InputIterator, concepts::ForwardIterator)
            {
                if(begin != end)
                {
                    *out = *begin;
                    while(++begin != end)
                        if(!pred(*out, *begin))
                            *++out = *begin;
                    ++out;
                }
                return out;
            }
        }

        struct uniquer_copier : bindable<uniquer_copier>
        {
            /// \brief template function unique_copy
            ///
            /// range-based version of the unique_copy std algorithm
            ///
            /// \pre InputIterable is a model of the InputIterable concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable, typename OutputIterator,
                CONCEPT_REQUIRES_(ranges::Iterable<InputIterable>())>
            static OutputIterator
            invoke(uniquer_copier, InputIterable && rng, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<InputIterable>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable>>());
                CONCEPT_ASSERT(
                    ((ranges::Iterable<InputIterable>() &&
                        ranges::ForwardIterator<range_iterator_t<InputIterable>>()) ||
                     ranges::ForwardIterator<OutputIterator>()) ||
                    (ranges::CopyConstructible<range_value_t<InputIterable>>() &&
                     ranges::CopyAssignable<range_value_t<InputIterable>>()));
                return detail::unique_copy(ranges::begin(rng), ranges::end(rng),
                    std::move(out), ranges::equal_to{}, iterator_concept_t<range_iterator_t<InputIterable>>{},
                    iterator_concept_t<OutputIterator>{});
            }

            /// \overload
            template<typename InputIterable, typename OutputIterator, typename BinaryPredicate>
            static OutputIterator
            invoke(uniquer_copier, InputIterable && rng, OutputIterator out,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                CONCEPT_ASSERT(ranges::InvokablePredicate<BinaryPredicate,
                                                          range_reference_t<InputIterable>,
                                                          range_reference_t<InputIterable>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable>>());
                CONCEPT_ASSERT(
                    ((ranges::Iterable<InputIterable>() &&
                        ranges::ForwardIterator<range_iterator_t<InputIterable>>()) ||
                     ranges::ForwardIterator<OutputIterator>()) ||
                    (ranges::CopyConstructible<range_value_t<InputIterable>>() &&
                     ranges::CopyAssignable<range_value_t<InputIterable>>()));
                return detail::unique_copy(ranges::begin(rng), ranges::end(rng),
                    std::move(out), ranges::make_invokable(std::move(pred)),
                    iterator_concept_t<range_iterator_t<InputIterable>>{},
                    iterator_concept_t<OutputIterator>{});
            }

            /// \overload
            /// for rng | unique_copy(out)
            template<typename OutputIterator>
            static auto
            invoke(uniquer_copier unique_copy, OutputIterator out) ->
                decltype(unique_copy.move_bind(std::placeholders::_1, std::move(out)))
            {
                return unique_copy.move_bind(std::placeholders::_1, std::move(out));
            }

            /// \overload
            /// for rng | unique_copy(out, pred)
            template<typename OutputIterator, typename BinaryPredicate,
                CONCEPT_REQUIRES_(!ranges::Iterable<OutputIterator>())>
            static auto
            invoke(uniquer_copier unique_copy, OutputIterator out, BinaryPredicate pred) ->
                decltype(unique_copy.move_bind(std::placeholders::_1, std::move(out), std::move(pred)))
            {
                return unique_copy.move_bind(std::placeholders::_1, std::move(out), std::move(pred));
            }
        };

        RANGES_CONSTEXPR uniquer_copier unique_copy {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
