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
#include <range/v3/concepts.hpp>
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
            invoke(replacer_if, ForwardRange && rng, UnaryPredicate pred, Value const & with_what)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::UnaryPredicate<UnaryPredicate, Value>());
                std::replace_if(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(detail::move(pred)), with_what);
                return std::forward<ForwardRange>(rng);
            }

            // BUGBUG should "rng | replace_if(pred, that)" be lazy? 

            /// \overload
            /// for rng | replace_if(pred, that)
            template<typename UnaryPredicate, typename Value>
            static auto invoke(replacer_if replace_if, UnaryPredicate pred, Value && with_what)
                -> decltype(replace_if(std::placeholders::_1, detail::move(pred),
                        ranges::ref_if_lvalue<Value>(with_what)))
            {
                return replace_if(std::placeholders::_1, detail::move(pred),
                    ranges::ref_if_lvalue<Value>(with_what));
            }
        };

        RANGES_CONSTEXPR replacer_if replace_if{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
