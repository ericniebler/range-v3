/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_AUX_PARTITION_POINT_N_HPP
#define RANGES_V3_ALGORITHM_AUX_PARTITION_POINT_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    namespace aux
    {
        struct partition_point_n_fn
        {
            template<typename I, typename C, typename P = identity>
            auto operator()(I begin, iter_difference_t<I> d, C pred, P proj = P{}) const ->
                CPP_ret(I)(
                    requires ForwardIterator<I> &&
                        IndirectUnaryPredicate<C, projected<I, P>>)
            {
                if(0 < d)
                {
                    do
                    {
                        auto half = d / 2;
                        auto middle = next(uncounted(begin), half);
                        if(invoke(pred, invoke(proj, *middle)))
                        {
                            begin = recounted(begin, std::move(++middle), half + 1);
                            d -= half + 1;
                        }
                        else
                            d = half;
                    } while(0 != d);
                }
                return begin;
            }
        };

        RANGES_INLINE_VARIABLE(partition_point_n_fn, partition_point_n)
    }
} // namespace ranges

#endif // include guard
