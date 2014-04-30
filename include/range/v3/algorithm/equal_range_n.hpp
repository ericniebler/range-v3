//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_EQUAL_RANGE_N_HPP
#define RANGES_V3_ALGORITHM_EQUAL_RANGE_N_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/lower_bound_n.hpp>
#include <range/v3/algorithm/upper_bound_n.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct equal_range_n_fn
        {
            template<typename I, typename V, typename R = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(BinarySearchable<I, V, R, P>())>
            iterator_range<I>
            operator()(I begin, iterator_difference_t<I> dist, V const & val, R pred_ = R{},
                P proj_ = P{}) const
            {
                RANGES_ASSERT(0 <= d);
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                while(0 != dist)
                {
                    auto half = dist / 2;
                    auto middle = next(begin, half);
                    if(pred(proj(*middle), val))
                    {
                        begin = std::move(++middle);
                        dist -= half + 1;
                    }
                    else if(pred(val, proj(*middle)))
                    {
                        dist = half;
                    }
                    else
                        return {lower_bound_n(std::move(begin), half, val, std::ref(pred)),
                                upper_bound_n(next(middle), dist - half - 1, val, std::ref(pred))};
                }
                return {begin, begin};
            }

            template<typename Rng, typename V, typename R = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && BinarySearchable<I, V, R, P>())>
            iterator_range<I>
            operator()(Rng & rng, iterator_difference_t<I> dist, V const & val, R pred = R{}, P proj = P{}) const
            {
                static_assert(!is_infinite<Rng>::value, "Trying to binary search an infinite range");
                RANGES_ASSERT(0 <= d);
                RANGES_ASSERT(d <= distance(rng));
                return (*this)(begin(rng), dist, std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<equal_range_n_fn> equal_range_n{};

    } // namespace v3
} // namespace ranges

#endif // include guard
