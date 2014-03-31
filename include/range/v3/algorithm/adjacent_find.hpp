//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP

#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct adjacent_find_fn
        {
            /// \brief function template \c adjacent_find_fn::operator()
            ///
            /// range-based version of the \c adjacent_find std algorithm
            ///
            /// \pre \c ForwardIterable is a model of the ForwardIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterator, typename Sentinel,
                typename BinaryPredicate = ranges::equal_to,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::ForwardIterator<ForwardIterator>() &&
                    ranges::Sentinel<Sentinel, ForwardIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<ForwardIterator>>() &&
                    ranges::InvokablePredicate<
                        BinaryPredicate,
                        concepts::Invokable::result_t<Projection, iterator_value_t<ForwardIterator>>,
                        concepts::Invokable::result_t<Projection, iterator_value_t<ForwardIterator>>>())>
            ForwardIterator
            operator()(ForwardIterator begin, Sentinel end,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                using namespace std::placeholders;
                auto &&ipred = std::bind(pred, std::bind(proj, _1), std::bind(proj, _2));
                if(begin == end)
                    return begin;
                auto next = begin;
                for(; ++next != end; begin = next)
                    if(ipred(*begin, *next))
                        return begin;
                return next;
            }

            /// \overload
            template<typename ForwardIterable,
                typename BinaryPredicate = ranges::equal_to,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::Iterable<ForwardIterable>() &&
                    ranges::ForwardIterator<range_iterator_t<ForwardIterable>>() &&
                    ranges::Invokable<Projection, range_value_t<ForwardIterable>>() &&
                    ranges::InvokablePredicate<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, range_value_t<ForwardIterable>>,
                        concepts::Invokable::result_t<Projection, range_value_t<ForwardIterable>>>())>
            range_iterator_t<ForwardIterable>
            operator()(ForwardIterable &rng,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), std::move(pred), std::move(proj));
            }

            /// \overload
            template<typename Value,
                typename BinaryPredicate = ranges::equal_to,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::Invokable<Projection, Value>() &&
                    ranges::InvokablePredicate<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, Value>,
                        concepts::Invokable::result_t<Projection, Value>>())>
            Value const *
            operator()(std::initializer_list<Value> const &rng,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR adjacent_find_fn adjacent_find {};

    } // namespace v3
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
