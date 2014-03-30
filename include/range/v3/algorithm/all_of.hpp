//  Copyright Andrew Sutton 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_ALL_OF_HPP
#define RANGES_V3_ALGORITHM_ALL_OF_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct all_of_fn
        {
            template<typename I, typename S, typename P>
            bool operator()(I first, S last, predicate<P> pred) const
            {
                while (first != last && pred(*first))
                    ++first;
                return first == last;
            }

            template<typename I, typename S, typename P,
                CONCEPT_REQUIRES_(ranges::InputIterator<I>() &&
                                  ranges::Sentinel<S, I>() &&
                                  ranges::AdaptablePredicate<P, iterator_value_t<I>>())>
            bool operator()(I first, S last, P pred) const
            {
                return (*this)(first, last, predicate<P>(pred));
            }

            template<typename I, typename P,
                CONCEPT_REQUIRES_(ranges::Iterable<I>() &&
                                  ranges::InputIterator<range_iterator_t<I>>() &&
                                  ranges::AdaptablePredicate<P, range_value_t<I>>())>
            bool operator()(const I& rng, P pred) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), pred);
            }

            template<typename T, typename P,
                CONCEPT_REQUIRES_(ranges::AdaptablePredicate<P, T>())>
            bool operator()(std::initializer_list<T> list, P pred) const
            {
                return (*this)(ranges::begin(list), ranges::end(list), pred);
            }        
        };

        RANGES_CONSTEXPR all_of_fn all_of {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
