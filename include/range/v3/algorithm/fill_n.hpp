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
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename Size, typename Value>
            void fill_n(ForwardIterator begin, Sentinel end, Size n, Value const & val)
            {
                for(; n != 0; --n, ++begin)
                {
                    RANGES_ASSERT(begin != end);
                    *begin = val;
                }
            }
        }

        struct filler_n : bindable<filler_n>
        {
            /// \brief template function \c filler_n::operator()
            ///
            /// range-based version of the \c fill_n std algorithm
            ///
            /// \pre \c ForwardIterable is a model of the ForwardIterable concept
            /// \pre <tt>n \<= std::distance(ranges::begin(rng), ranges::end(rng))</tt>
            template<typename ForwardIterable, typename Size, typename Value>
            static ForwardIterable invoke(filler_n, ForwardIterable && rng, Size n, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable>());
                detail::fill_n(ranges::begin(rng), ranges::end(rng), n, val);
                return std::forward<ForwardIterable>(rng);
            }

            /// \overload
            template<typename Size, typename Value>
            static auto invoke(filler_n fill_n, Size n, Value && val) ->
                decltype(fill_n.move_bind(std::placeholders::_1, n, std::forward<Value>(val)))
            {
                return fill_n.move_bind(std::placeholders::_1, n, std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR filler_n fill_n {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
