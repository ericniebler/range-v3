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
#ifndef RANGES_V3_ALGORITHM_REMOVE_HPP
#define RANGES_V3_ALGORITHM_REMOVE_HPP

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
        struct remover : bindable<remover>
        {
            /// \brief template function remove
            ///
            /// range-based version of the remove std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename Value>
            static range_iterator_t<ForwardRange>
            invoke(remover, ForwardRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return std::remove(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            /// for rng | remove(val)
            template<typename Value>
            static auto invoke(remover remove, Value val)
                -> decltype(remove(std::placeholders::_1, detail::move(val)))
            {
                return remove(std::placeholders::_1, detail::move(val));
            }
        };

        RANGES_CONSTEXPR remover remove {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
