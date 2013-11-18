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
#ifndef RANGES_V3_ALGORITHM_FILL_HPP
#define RANGES_V3_ALGORITHM_FILL_HPP

#include <utility>
#include <algorithm>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct filler
        {
            /// \brief template function \c filler::operator()
            ///
            /// range-based version of the \c fill std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename Value>
            ForwardRange operator()(ForwardRange && rng, Value const & val) const
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                std::fill(ranges::begin(rng), ranges::end(rng), val);
                return std::forward<ForwardRange>(rng);
            }

            /// \overload
            template<typename Value, typename This = filler>
            auto operator()(Value const & val) const
                -> decltype(bindable<This>{}(std::placeholders::_1, val))
            {
                return bindable<This>{}(std::placeholders::_1, val);
            }
        };

        constexpr bindable<filler> fill {};

    } // inline namespace v3
}

#endif // include guard
