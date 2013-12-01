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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_HPP

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
        struct replacer_copier : bindable<replacer_copier>
        {
            /// \brief template function replace_copy
            ///
            /// range-based version of the replace_copy std algorithm
            ///
            /// \pre ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename OutputIterator, typename Value>
            static OutputIterator
            invoke(replacer_copier, ForwardRange && rng, OutputIterator out_it,
                   Value const & what, Value const & with_what)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator, Value>());
                return std::replace_copy(ranges::begin(rng), ranges::end(rng),
                    detail::move(out_it), what, with_what);
            }

            // BUGBUG should "rng | replace_copy(this, that)" be lazy? 

            /// \overload
            /// for rng | replace_copy(out, that, that)
            template<typename OutputIterator, typename Value>
            static auto invoke(replacer_copier replace_copy, OutputIterator out_it, Value what, Value with_what)
                -> decltype(replace_copy(std::placeholders::_1, detail::move(out_it),
                        detail::move(what), detail::move(with_what)))
            {
                return replace_copy(std::placeholders::_1, detail::move(out_it),
                    detail::move(what), detail::move(with_what));
            }
        };

        RANGES_CONSTEXPR replacer_copier replacer_copy{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
