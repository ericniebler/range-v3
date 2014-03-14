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
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_IF_HPP

#include <utility>
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
            template<typename InputIterator, typename Sentinel,
                     typename OutputIterator, typename UnaryPredicate>
            OutputIterator
            remove_copy_if(InputIterator begin, Sentinel end,
                           OutputIterator out, UnaryPredicate pred)
            {
                for (; begin != end; ++begin)
                    if (!pred(*begin))
                        *out++ = *begin;
                return out;
            }
        }

        struct remover_copier_if : bindable<remover_copier_if>
        {
            /// \brief template function remove_copy_if
            ///
            /// range-based version of the remove_copy_if std algorithm
            ///
            /// \pre InputIterable is a model of the InputIterable concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre UnaryPredicate is a model of the UnaryPredicate concept
            /// \pre InputIterator's value type is convertible to Predicate's argument type
            /// \pre out is not an iterator in the range rng
            template<typename InputIterable, typename OutputIterator, typename UnaryPredicate>
            static OutputIterator
            invoke(remover_copier_if, InputIterable && rng, OutputIterator out, UnaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                return detail::remove_copy_if(ranges::begin(rng), ranges::end(rng),
                    std::move(out), ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | remove_copy_if(out, pred)
            template<typename OutputIterator, typename UnaryPredicate>
            static auto
            invoke(remover_copier_if remove_copy_if, OutputIterator out, UnaryPredicate pred) ->
                decltype(remove_copy_if.move_bind(std::placeholders::_1, std::move(out),
                    std::move(pred)))
            {
                return remove_copy_if.move_bind(std::placeholders::_1, std::move(out),
                    std::move(pred));
            }
        };

        RANGES_CONSTEXPR remover_copier_if remove_copy_if {};
    }
}

#endif // include guard
