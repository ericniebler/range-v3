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
#ifndef RANGES_V3_ALGORITHM_GENERATE_HPP
#define RANGES_V3_ALGORITHM_GENERATE_HPP

#include <utility>
#include <algorithm>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct generator
        {
            /// \brief template function generate
            ///
            /// range-based version of the generate std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            /// \pre Generator is a model of the UnaryFunction concept
            template<typename ForwardRange, typename Generator>
            ForwardRange operator()(ForwardRange && rng, Generator gen) const
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                std::generate(ranges::begin(rng), ranges::end(rng), detail::move(gen));
                return detail::forward<ForwardRange>(rng);
            }

            /// \overload
            template<typename Generator, typename This = generator>
            auto operator()(Generator gen) const
                -> decltype(bindable<This>{}(std::placeholders::_1, detail::move(gen)))
            {
                return bindable<This>{}(std::placeholders::_1, detail::move(gen));
            }
        };

        constexpr bindable<generator> generate {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
