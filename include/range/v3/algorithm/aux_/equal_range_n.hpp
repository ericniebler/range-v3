/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_AUX_EQUAL_RANGE_N_HPP
#define RANGES_V3_ALGORITHM_AUX_EQUAL_RANGE_N_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/aux_/lower_bound_n.hpp>
#include <range/v3/algorithm/aux_/upper_bound_n.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux
        {
            struct equal_range_n_fn
            {
                template<typename I, typename V, typename R = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BinarySearchable<I, V, R, P>())>
                iterator_range<I>
                operator()(I begin, difference_type_t<I> dist, V const & val, R pred = R{},
                    P proj = P{}) const
                {
                    if(0 < dist)
                    {
                        do
                        {
                            auto half = dist / 2;
                            auto middle = next(begin, half);
                            auto && v = *middle;
                            auto && pv = invoke(proj, (decltype(v) &&) v);
                            if(invoke(pred, pv, val))
                            {
                                begin = std::move(++middle);
                                dist -= half + 1;
                            }
                            else if(invoke(pred, val, pv))
                            {
                                dist = half;
                            }
                            else
                            {
                                return {
                                    lower_bound_n(std::move(begin), half, val,
                                        std::ref(pred), std::ref(proj)),
                                    upper_bound_n(next(middle), dist - (half + 1),
                                        val, std::ref(pred), std::ref(proj))
                                };
                            }
                        } while(0 != dist);
                    }
                    return {begin, begin};
                }
            };

            RANGES_INLINE_VARIABLE(equal_range_n_fn, equal_range_n)
        }
    } // namespace v3
} // namespace ranges

#endif // include guard
