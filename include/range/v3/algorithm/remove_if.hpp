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
#ifndef RANGES_V3_ALGORITHM_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_IF_HPP

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
        struct remover_if : bindable<remover_if>
        {
            /// \brief template function remove_if
            ///
            /// range-based version of the remove_if std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre UnaryPredicate is a model of the UnaryPredicate concept
            template<typename ForwardRange, typename UnaryPredicate>
            static range_iterator_t<ForwardRange>
            invoke(remover_if, ForwardRange && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::remove_if(ranges::begin(rng), ranges::end(rng), detail::move(pred));
            }

            /// \overload
            /// for rng | remove(pred)
            template<typename UnaryPredicate>
            static auto invoke(remover_if remove_if, UnaryPredicate pred)
                -> decltype(remove_if(std::placeholders::_1, detail::move(pred)))
            {
                return remove_if(std::placeholders::_1, detail::move(pred));
            }
        };

        RANGES_CONSTEXPR remover_if remove_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
