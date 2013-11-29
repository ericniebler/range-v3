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
#ifndef RANGES_V3_ALGORITHM_REPLACE_HPP
#define RANGES_V3_ALGORITHM_REPLACE_HPP

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
        struct replacer : bindable<replacer>
        {
            /// \brief template function replace
            ///
            /// range-based version of the replace std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename Value>
            static ForwardRange
            invoke(replacer, ForwardRange && rng, Value const & what, Value const & with_what)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                std::replace(ranges::begin(rng), ranges::end(rng), what, with_what);
                return std::forward<ForwardRange>(rng);
            }

            // BUGBUG should "rng | replace(this, that)" be lazy? 

            /// \overload
            /// for rng | replace(that, that)
            template<typename Value>
            static auto invoke(replacer replace, Value what, Value with_what)
                -> decltype(replace(std::placeholders::_1, detail::move(what), detail::move(with_what)))
            {
                return replace(std::placeholders::_1, detail::move(what), detail::move(with_what));
            }
        };

        constexpr replacer replace{};

    } // inline namespace v3

} // namespace ranges;

#endif // include guard
