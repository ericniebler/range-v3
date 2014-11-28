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
#ifndef RANGES_V3_ALGORITHM_AUX_UPPER_BOUND_N_HPP
#define RANGES_V3_ALGORITHM_AUX_UPPER_BOUND_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux
        {
            struct upper_bound_n_fn
            {
                /// \brief template function upper_bound
                ///
                /// range-based version of the upper_bound std algorithm
                ///
                /// \pre Rng is a model of the Range concept
                template<typename I, typename V2, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BinarySearchable<I, V2, C, P>())>
                I operator()(I begin, iterator_difference_t<I> d, V2 const &val, C pred_ = C{},
                    P proj_ = P{}) const
                {
                    RANGES_ASSERT(0 <= d);
                    auto &&pred = invokable(pred_);
                    auto &&proj = invokable(proj_);
                    while(0 != d)
                    {
                        auto half = d / 2;
                        auto middle = next(begin, half);
                        if(pred(val, proj(*middle)))
                            d = half;
                        else
                        {
                            begin = std::move(++middle);
                            d -= half + 1;
                        }
                    }
                    return begin;
                }
            };

            constexpr upper_bound_n_fn upper_bound_n {};
        }
    } // namespace v3
} // namespace ranges

#endif // include guard
