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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP

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

        struct replacer_copier_if : bindable<replacer_copier_if>
        {
            /// \brief template function replace_copy_if
            ///
            /// range-based version of the replace_copy_if std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre UnaryPredicate is a model of the UnaryPredicate concept
            /// \pre Value is convertible to UnaryPredicate's argument type
            /// \pre Value is CopyAssignable
            /// \pre Value is convertible to a type in OutputIterator's set of value types.
            template<typename ForwardRange, typename OutputIterator, typename UnaryPredicate,
                typename Value>
            static OutputIterator
            invoke(replacer_copier_if, ForwardRange && rng, OutputIterator out, UnaryPredicate pred,
                Value const & new_value)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<ForwardRange>>());
                CONCEPT_ASSERT(ranges::UnaryPredicate<invokable_t<UnaryPredicate>,
                                                      range_reference_t<ForwardRange>>());
                return std::replace_copy_if(ranges::begin(rng), ranges::end(rng),
                    std::move(out), ranges::make_invokable(std::move(pred)), new_value);
            }

            // BUGBUG should "rng | replace_copy_if(out, pred, that)" be lazy?

            /// \overload
            /// for rng | replace_copy_if(out, pred, that)
            template<typename OutputIterator, typename UnaryPredicate, typename Value>
            static auto invoke(replacer_copier_if replace_copy_if, OutputIterator out,
                               UnaryPredicate pred, Value && new_value)
                -> decltype(replace_copy_if.move_bind(std::placeholders::_1, std::move(out),
                            std::move(pred), std::forward<Value>(new_value)))
            {
                return replace_copy_if.move_bind(std::placeholders::_1, std::move(out),
                    std::move(pred), std::forward<Value>(new_value));
            }
        };

        RANGES_CONSTEXPR replacer_copier_if replacer_copy_if{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
