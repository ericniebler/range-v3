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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_IS_PARTITIONED_HPP
#define RANGES_V3_ALGORITHM_IS_PARTITIONED_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        CPP_def
        (
            template(typename I, typename C, typename P = identity)
            (concept IsPartitionedable)(I, C, P),
                InputIterator<I> &&
                IndirectPredicate<C, projected<I, P>>
        );

        /// \addtogroup group-algorithms
        /// @{
        struct is_partitioned_fn
        {
            template<typename I, typename S, typename C, typename P = identity>
            auto operator()(I begin, S end, C pred, P proj = P{}) const ->
                CPP_ret(bool)(
                    requires IsPartitionedable<I, C, P> && Sentinel<S, I>)
            {
                for(; begin != end; ++begin)
                    if(!invoke(pred, invoke(proj, *begin)))
                        break;
                for(; begin != end; ++begin)
                    if(invoke(pred, invoke(proj, *begin)))
                        return false;
                return true;
            }

            template<typename Rng, typename C, typename P = identity>
            auto operator()(Rng &&rng, C pred, P proj = P{}) const ->
                CPP_ret(bool)(
                    requires IsPartitionedable<iterator_t<Rng>, C, P> && Range<Rng>)
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `is_partitioned_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<is_partitioned_fn>,
                               is_partitioned)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
