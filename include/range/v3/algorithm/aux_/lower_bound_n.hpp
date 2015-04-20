/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_AUX_LOWER_BOUND_N_HPP
#define RANGES_V3_ALGORITHM_AUX_LOWER_BOUND_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux
        {
            struct lower_bound_n_fn
            {
                template<typename I, typename V2, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BinarySearchable<I, V2, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                I operator()(I begin, iterator_difference_t<I> d, V2 const &val, C pred_ = C{},
                    P proj_ = P{}) const
                {
                    RANGES_ASSERT(0 <= d);
                    auto &&pred = as_function(pred_);
                    auto &&proj = as_function(proj_);
                    while(0 != d)
                    {
                        auto half = d / 2;
                        auto middle = next(begin, half);
                        if(pred(proj(*middle), val))
                        {
                            begin = std::move(++middle);
                            d -= half + 1;
                        }
                        else
                            d = half;
                    }
                    return begin;
                }
            };

            namespace
            {
                constexpr auto&& lower_bound_n = static_const<lower_bound_n_fn>::value;
            }
        }
    } // namespace v3
} // namespace ranges

#endif // include guard
