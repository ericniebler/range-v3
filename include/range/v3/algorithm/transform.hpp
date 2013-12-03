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
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator1,
                     typename InputIterator2,
                     typename OutputIterator,
                     typename BinaryFunction>
            OutputIterator
            transform_impl(InputIterator1 first1,
                           InputIterator1 last1,
                           InputIterator2 first2,
                           InputIterator2 last2,
                           OutputIterator out,
                           BinaryFunction fun)
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
            template<typename InputRange1, typename OutputIterator, typename UnaryFunction>
            static OutputIterator
            invoke(transformer, InputRange1 && rng, OutputIterator out, UnaryFunction fun)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                using Ref1 = range_reference_t<InputRange1>;
                CONCEPT_ASSERT(ranges::Callable<UnaryFunction, Ref1>());
                using Ref2 = result_of_t<UnaryFunction(Ref1)>;
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator, Ref2>());
                return std::transform(ranges::begin(rng), ranges::end(rng),
                                      detail::move(out), detail::move(fun));
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
                using Ref1 = range_reference_t<InputRange1>;
                using Ref2 = range_reference_t<InputRange2>;
                CONCEPT_ASSERT(ranges::Callable<BinaryFunction, Ref1, Ref2>());
                using Value3 = result_of_t<BinaryFunction(Ref1, Ref2)>;
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator, Value3>());
                return detail::transform_impl(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    detail::move(out), detail::move(fun));
            }
        };

        RANGES_CONSTEXPR transformer transform {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
