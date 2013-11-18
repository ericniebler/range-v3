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
#ifndef RANGES_V3_ALGORITHM_FILL_N_HPP
#define RANGES_V3_ALGORITHM_FILL_N_HPP

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
        struct filler_n
        {
            /// \brief template function \c filler_n::operator()
            ///
            /// range-based version of the \c fill_n std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            /// \pre <tt>n \<= std::distance(ranges::begin(rng), ranges::end(rng))</tt>
            template<typename ForwardRange, typename Size, typename Value>
            ForwardRange operator()(ForwardRange && rng, Size n, Value const & val) const
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                RANGES_ASSERT(static_cast<Size>(std::distance(ranges::begin(rng), ranges::end(rng))) >= n);
                std::fill_n(ranges::begin(rng), n, val);
                return std::forward<ForwardRange>(rng);
            }

            /// \overload
            // BUGBUG worry about lifetime issue of val here. Should use perfect
            // forwarding. Must fix bindable to do perfect forwarding to std::bind.
            template<typename Size, typename Value, typename This = filler>
            auto operator()(Size n, Value const & val) const
                -> decltype(bindable<This>{}(std::placeholders::_1, n, val))
            {
                return bindable<This>{}(std::placeholders::_1, n, val);
            }
        };

        constexpr binder<filler_n> fill_n {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
