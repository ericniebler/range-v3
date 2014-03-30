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
#ifndef RANGES_V3_ALGORITHM_TRANSFORM_HPP
#define RANGES_V3_ALGORITHM_TRANSFORM_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator, typename Sentinel,
                     typename OutputIterator, typename UnaryFunction>
            OutputIterator
            transform(InputIterator begin, Sentinel end, OutputIterator out, UnaryFunction fun)
            {
                for(; begin != end; ++begin, ++out)
                    *out = fun(*begin);
                return out;
            }

            template<typename InputIterator1, typename Sentinel1,
                     typename InputIterator2, typename Sentinel2,
                     typename OutputIterator, typename BinaryFunction>
            OutputIterator
            transform(InputIterator1 begin1, Sentinel1 end1,
                      InputIterator2 begin2, Sentinel2 end2,
                      OutputIterator out, BinaryFunction fun)
            {
                for (; begin1 != end1; ++begin1, ++begin2, ++out)
                {
                    RANGE_ASSERT(begin2 != end2);
                    *out = fun(*begin1, *begin2);
                }
                return out;
            }
        }

        struct transformer : bindable<transformer>
        {
            /// \brief template function transform
            ///
            /// range-based version of the transform std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre UnaryFunction is a model of the UnaryFunction concept
            /// \pre BinaryFunction is a model of the BinaryFunction concept
            template<typename InputIterable1, typename OutputIterator, typename UnaryFunction,
                CONCEPT_REQUIRES_(
                    ranges::Iterable<InputIterable1>() &&
                    ranges::InputIterator<range_iterator_t<InputIterable1>>() &&
                    ranges::Invokable<UnaryFunction,
                                      range_reference_t<InputIterable1>>() &&
                    ranges::OutputIterator<OutputIterator,
                        concepts::Invokable::result_t<UnaryFunction,
                                                      range_reference_t<InputIterable1>>>())>
            static OutputIterator
            invoke(transformer, InputIterable1 && rng, OutputIterator out, UnaryFunction fun)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1>>());
                CONCEPT_ASSERT(ranges::Invokable<UnaryFunction,
                                                 range_reference_t<InputIterable1>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                    concepts::Invokable::result_t<UnaryFunction,
                                                  range_reference_t<InputIterable1>>>());
                return detail::transform(
                    ranges::begin(rng), ranges::end(rng),
                    std::move(out), ranges::make_invokable(std::move(fun)));
            }

            /// \overload
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator,
                     typename BinaryFunction>
            static OutputIterator
            invoke(transformer, InputIterable1 && rng1, InputIterable2 && rng2, OutputIterator out,
                   BinaryFunction fun)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2>>());
                CONCEPT_ASSERT(ranges::Invokable<BinaryFunction,
                                                 range_reference_t<InputIterable1>,
                                                 range_reference_t<InputIterable2>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                    concepts::Invokable::result_t<BinaryFunction,
                                                  range_reference_t<InputIterable1>,
                                                  range_reference_t<InputIterable2>>>());
                return detail::transform(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    std::move(out), ranges::make_invokable(std::move(fun)));
            }

            /// \overload
            /// for rng | transform(out, uni_fun)
            template<typename OutputIterator, typename UnaryFunction>
            static auto
            invoke(transformer transform, OutputIterator out, UnaryFunction fun) ->
                decltype(transform.move_bind(std::placeholders::_1, std::move(out), std::move(fun)))
            {
                return transform.move_bind(std::placeholders::_1, std::move(out), std::move(fun));
            }

            /// \overload
            /// for rng | transform(rng2, out, bin_fun)
            template<typename InputIterable2, typename OutputIterator, typename BinaryFunction,
                CONCEPT_REQUIRES_(!(
                    ranges::Iterable<InputIterable2>() &&
                    ranges::InputIterator<range_iterator_t<InputIterable2>>() &&
                    ranges::Invokable<BinaryFunction,
                                      range_reference_t<InputIterable2>>() &&
                    ranges::OutputIterator<OutputIterator,
                        concepts::Invokable::result_t<BinaryFunction,
                                                      range_reference_t<InputIterable2>>>()))>
            static auto
            invoke(transformer transform, InputIterable2 && rng2, OutputIterator out,
                   BinaryFunction fun) ->
                   decltype(transform.move_bind(std::placeholders::_1, std::forward<InputIterable2>(rng2),
                    std::move(out), std::move(fun)))
            {
                return transform.move_bind(std::placeholders::_1, std::forward<InputIterable2>(rng2),
                    std::move(out), std::move(fun));
            }
        };

        RANGES_CONSTEXPR transformer transform {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
