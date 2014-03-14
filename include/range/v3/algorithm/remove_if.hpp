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
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
            ForwardIterator
            remove_if(ForwardIterator begin, Sentinel end, UnaryPredicate pred)
            {
                begin = detail::find_if(std::move(begin), end, std::ref(pred));
                if(begin != end)
                    for(auto tmp = ranges::next(begin); tmp != end; ++tmp)
                        if(!pred(*tmp))
                            *begin++ = std::move(*tmp);
                return begin;
            }
        }

        struct remover_if : bindable<remover_if>
        {
            /// \brief template function remove_if
            ///
            /// range-based version of the remove_if std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            /// \pre UnaryPredicate is a model of the UnaryPredicate concept
            template<typename ForwardIterable, typename UnaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(remover_if, ForwardIterable && rng, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                return detail::remove_if(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | remove(pred)
            template<typename UnaryPredicate>
            static auto invoke(remover_if remove_if, UnaryPredicate pred) ->
                decltype(remove_if.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return remove_if.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR remover_if remove_if {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
