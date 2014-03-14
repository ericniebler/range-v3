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
#ifndef RANGES_V3_ALGORITHM_UNIQUE_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_HPP

#include <utility>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename Sentinel,
                     typename BinaryPredicate = ranges::equal_to>
            ForwardIterator
            unique(ForwardIterator begin, Sentinel end, BinaryPredicate pred = BinaryPredicate{})
            {
                begin = detail::adjacent_find(std::move(begin), end, std::ref(pred));
                if(begin != end)
                {
                    for(auto tmp = ranges::next(begin, 2); tmp != end; ++tmp)
                        if(!pred(*begin, *tmp))
                            *++begin = std::move(*tmp);
                    ++begin;
                }
                return begin;
            }
        }

        struct uniquer : bindable<uniquer>, pipeable<uniquer>
        {
            /// \brief template function unique
            ///
            /// range-based version of the unique std algorithm
            ///
            /// \pre Rng meets the requirements for a Forward range
            template<typename ForwardIterable,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>())>
            static range_iterator_t<ForwardIterable>
            invoke(uniquer, ForwardIterable && rng)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::MoveAssignable<range_reference_t<ForwardIterable>>());
                return detail::unique(ranges::begin(rng), ranges::end(rng));
            }

            /// \overload
            template<typename ForwardIterable, typename BinaryPredicate>
            static range_iterator_t<ForwardIterable>
            invoke(uniquer, ForwardIterable && rng, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardIterable>,
                                                       range_reference_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::MoveAssignable<range_reference_t<ForwardIterable>>());
                return detail::unique(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | unique(pred)
            template<typename BinaryPredicate,
                CONCEPT_REQUIRES_(!ranges::Iterable<BinaryPredicate>())>
            static auto
            invoke(uniquer unique, BinaryPredicate pred) ->
                decltype(unique.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return unique.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR uniquer unique {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
