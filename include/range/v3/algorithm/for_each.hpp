//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <functional>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct for_each_fn
        {
            template<typename InputIterator, typename Sentinel, typename Fun,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::InputIterator<InputIterator>() &&
                    ranges::Sentinel<Sentinel, InputIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<InputIterator>>() &&
                    ranges::Invokable<Fun,
                        concepts::Invokable::result_t<Projection, iterator_value_t<InputIterator>>>())>
            InputIterator
            operator()(InputIterator begin, Sentinel end, Fun fun, Projection proj = Projection{}) const
            {
                using namespace std::placeholders;
                auto &&ifun = std::bind(fun, std::bind(proj, _1));
                for(; begin != end; ++begin)
                {
                    ifun(*begin);
                }
                return begin;
            }

            template<typename InputIterable, typename Fun,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::Iterable<InputIterable>() &&
                    ranges::InputIterator<range_iterator_t<InputIterable>>() &&
                    ranges::Invokable<Projection, range_value_t<InputIterable>>() &&
                    ranges::Invokable<Fun,
                        concepts::Invokable::result_t<Projection, range_value_t<InputIterable>>>())>
            range_iterator_t<InputIterable>
            operator()(InputIterable &rng, Fun fun, Projection proj = Projection{}) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), std::move(fun), std::move(proj));
            }

            template<typename Value, typename Fun,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::Invokable<Projection, Value>() &&
                    ranges::Invokable<Fun, concepts::Invokable::result_t<Projection, Value>>())>
            Value const *
            operator()(std::initializer_list<Value> const &rng, Fun fun, Projection proj = Projection{}) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), std::move(fun), std::move(proj));
            }
        };

        RANGES_CONSTEXPR for_each_fn for_each{};
    } // namespace v3
} // namespace ranges

#endif // include guard
