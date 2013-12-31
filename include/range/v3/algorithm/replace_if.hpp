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
#ifndef RANGES_V3_ALGORITHM_REPLACE_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_IF_HPP

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
        struct replacer_if : bindable<replacer_if>
        {
            /// \brief template function replace_if
            ///
            /// range-based version of the replace_if std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre UnaryPredicate is a model of the UnaryPredicate concept
            template<typename ForwardRange, typename UnaryPredicate, typename Value>
            static ForwardRange
            invoke(replacer_if, ForwardRange && rng, UnaryPredicate pred, Value const & new_value)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::UnaryPredicate<invokable_t<UnaryPredicate>,
                                                      range_reference_t<ForwardRange>>());
                std::replace_if(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)), new_value);
                return std::forward<ForwardRange>(rng);
            }

            /// \overload
            /// for rng | replace_if(pred, that)
            template<typename UnaryPredicate, typename Value>
            static auto invoke(replacer_if replace_if, UnaryPredicate pred, Value && new_value)
                -> decltype(replace_if.move_bind(std::placeholders::_1, std::move(pred),
                        std::forward<Value>(new_value)))
            {
                return replace_if.move_bind(std::placeholders::_1, std::move(pred),
                    std::forward<Value>(new_value));
            }
        };

        RANGES_CONSTEXPR replacer_if replace_if{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
