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
#ifndef RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP
#define RANGES_V3_ALGORITHM_FIND_FIRST_OF_HPP

#include <utility>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator1, typename Sentinel1,
                     typename ForwardIterator2, typename Sentinel2,
                     typename BinaryPredicate = ranges::equal_to>
            InputIterator1
            find_first_of(InputIterator1 begin1, Sentinel1 end1,
                          ForwardIterator2 begin2, Sentinel2 end2,
                          BinaryPredicate pred = BinaryPredicate{})
            {
                for(; begin1 != end1; ++begin1)
                    for(auto tmp = begin2; tmp != end2; ++tmp)
                        if(pred(*begin1, *tmp))
                            return begin1;
                return begin1;
            }
        }

        struct first_of_finder : bindable<first_of_finder>
        {
            /// \brief template function \c first_of_finder::operator()
            ///
            /// range-based version of the \c find_first_of std algorithm
            ///
            /// \pre \c InputIterable1 is a model of the InputIterable concept
            /// \pre \c ForwardIterable2 is a model of the ForwardIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename ForwardIterable2,
                CONCEPT_REQUIRES(ranges::Iterable<InputIterable1>() &&
                                 ranges::Iterable<ForwardIterable2 const>())>
            static range_iterator_t<InputIterable1>
            invoke(first_of_finder, InputIterable1 && rng1, ForwardIterable2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2 const>());
                CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<InputIterable1>,
                                                          range_reference_t<ForwardIterable2 const>>());
                return detail::find_first_of(ranges::begin(rng1), ranges::end(rng1),
                                             ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputIterable1, typename ForwardIterable2, typename BinaryPredicate>
            static range_iterator_t<InputIterable1>
            invoke(first_of_finder, InputIterable1 && rng1, ForwardIterable2 const & rng2,
                   BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputIterable<InputIterable1>());
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2 const>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<BinaryPredicate,
                                                       range_reference_t<InputIterable1>,
                                                       range_reference_t<ForwardIterable2 const>>());
                return detail::find_first_of(ranges::begin(rng1), ranges::end(rng1),
                                             ranges::begin(rng2), ranges::end(rng2),
                                             ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | find_first_of(rng2)
            template<typename ForwardIterable2>
            static auto
            invoke(first_of_finder find_first_of, ForwardIterable2 && rng2) ->
                decltype(find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2>());
                return find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2));
            }

            /// \overload
            /// for rng | find_first_of(rng2, pred)
            template<typename ForwardIterable2, typename BinaryPredicate,
                CONCEPT_REQUIRES(ranges::Iterable<ForwardIterable2>() &&
                                !ranges::Iterable<BinaryPredicate>())>
            static auto
            invoke(first_of_finder find_first_of, ForwardIterable2 && rng2, BinaryPredicate pred) ->
                decltype(find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2), std::move(pred)))
            {
                CONCEPT_ASSERT(ranges::ForwardIterable<ForwardIterable2>());
                return find_first_of.move_bind(std::placeholders::_1, std::forward<ForwardIterable2>(rng2), std::move(pred));
            }
        };

        RANGES_CONSTEXPR first_of_finder find_first_of {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
