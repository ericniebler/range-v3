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
#ifndef RANGES_V3_ALGORITHM_FIND_IF_HPP
#define RANGES_V3_ALGORITHM_FIND_IF_HPP

#include <utility>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
            ForwardIterator
            find_if(ForwardIterator begin, Sentinel end, UnaryPredicate pred)
            {
                for(; begin != end; ++begin)
                    if(pred(*begin))
                        break;
                return begin;
            }
        }

        struct finder_if : bindable<finder_if>
        {
            /// \brief template function \c finder_if::operator()
            ///
            /// range-based version of the \c find_if std algorithm
            ///
            /// \pre \c InputIterable is a model of the InputIterable concept
            /// \pre \c UnaryPredicate is a model of the UnaryPredicate concept
            template<typename InputIterable, typename UnaryPredicate>
            static range_iterator_t<InputIterable>
            invoke(finder_if, InputIterable && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                CONCEPT_ASSERT(ranges::UnaryPredicate<invokable_t<UnaryPredicate>,
                                                      range_reference_t<InputIterable>>());
                return detail::find_if(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            template<typename UnaryPredicate>
            static auto invoke(finder_if find_if, UnaryPredicate pred) ->
                decltype(find_if.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return find_if.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR finder_if find_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
