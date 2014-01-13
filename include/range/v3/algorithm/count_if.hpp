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
#ifndef RANGES_V3_ALGORITHM_COUNT_IF_HPP
#define RANGES_V3_ALGORITHM_COUNT_IF_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
            iterator_difference_t<InputIterator>
            count_if(InputIterator begin, Sentinel end, UnaryPredicate pred)
            {
                iterator_difference_t<InputIterator> n = 0;
                for(; begin != end; ++begin)
                    if(pred(*begin))
                        ++n;
                return n;
            }
        }

        struct counter_if : bindable<counter_if>
        {
            /// \brief template function \c counter_if::operator()
            ///
            /// range-based version of the \c count_if std algorithm
            ///
            /// \pre \c InputIterable is a model of the InputIterable concept
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename InputIterable, typename UnaryPredicate>
            static range_difference_t<InputIterable>
            invoke(counter_if, InputIterable && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                return detail::count_if(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \brief template function \c counter::operator()
            ///
            /// range-based version of the \c count std algorithm
            ///
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename UnaryPredicate>
            static auto invoke(counter_if count_if, UnaryPredicate pred)
                -> decltype(count_if.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return count_if.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR counter_if count_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
