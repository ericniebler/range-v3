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
            template<typename InputIterator1, typename InputIterator2,
                     typename OutputIterator, typename BinaryFunction>
            OutputIterator
            transform_impl(InputIterator1 first1, InputIterator1 last1,
                           InputIterator2 first2, InputIterator2 last2,
                           OutputIterator out, BinaryFunction fun)
            {
                for (; first1 != last1; ++first1, ++first2)
                {
                    RANGE_ASSERT(first2 != last2);
                    *out = fun(*first1, *first2);
                    ++out;
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
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre UnaryFunction is a model of the UnaryFunction concept
            /// \pre BinaryFunction is a model of the BinaryFunction concept
            template<typename InputRange1, typename OutputIterator, typename UnaryFunction,
                CONCEPT_REQUIRES(
                    ranges::InputRange<InputRange1>() &&
                    ranges::Callable<invokable_t<UnaryFunction>,
                                                 range_reference_t<InputRange1>>() &&
                    ranges::OutputIterator<OutputIterator,
                        concepts::Callable::result_t<invokable_t<UnaryFunction>,
                                                     range_reference_t<InputRange1>>>())>
            static OutputIterator
            invoke(transformer, InputRange1 && rng, OutputIterator out, UnaryFunction fun)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::Callable<invokable_t<UnaryFunction>,
                                                range_reference_t<InputRange1>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                    concepts::Callable::result_t<invokable_t<UnaryFunction>,
                                                 range_reference_t<InputRange1>>>());
                return std::transform(ranges::begin(rng), ranges::end(rng),
                                      std::move(out),
                                      ranges::make_invokable(std::move(fun)));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename OutputIterator,
                     typename BinaryFunction>
            static OutputIterator
            invoke(transformer, InputRange1 && rng1, InputRange2 && rng2, OutputIterator out,
                   BinaryFunction fun)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2>());
                CONCEPT_ASSERT(ranges::Callable<invokable_t<BinaryFunction>,
                                                range_reference_t<InputRange1>,
                                                range_reference_t<InputRange2>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                    concepts::Callable::result_t<invokable_t<BinaryFunction>,
                                                 range_reference_t<InputRange1>,
                                                 range_reference_t<InputRange2>>>());
                return detail::transform_impl(ranges::begin(rng1), ranges::end(rng1),
                                              ranges::begin(rng2), ranges::end(rng2),
                                              std::move(out),
                                              ranges::make_invokable(std::move(fun)));
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
            template<typename InputRange2, typename OutputIterator, typename BinaryFunction,
                CONCEPT_REQUIRES(!(
                    ranges::InputRange<InputRange2>() &&
                    ranges::Callable<invokable_t<BinaryFunction>,
                                                 range_reference_t<InputRange2>>() &&
                    ranges::OutputIterator<OutputIterator,
                        concepts::Callable::result_t<invokable_t<BinaryFunction>,
                                                     range_reference_t<InputRange2>>>()))>
            static auto
            invoke(transformer transform, InputRange2 && rng2, OutputIterator out,
                   BinaryFunction fun) ->
                   decltype(transform.move_bind(std::placeholders::_1, std::forward<InputRange2>(rng2),
                    std::move(out), std::move(fun)))
            {
                return transform.move_bind(std::placeholders::_1, std::forward<InputRange2>(rng2),
                    std::move(out), std::move(fun));
            }
        };

        RANGES_CONSTEXPR transformer transform {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
