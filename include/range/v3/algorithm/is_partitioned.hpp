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
        template<typename I, typename C, typename P = ident>
        using IsPartitionedable = meta::fast_and<
            InputIterator<I>,
            IndirectCallablePredicate<C, Project<I, P>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct is_partitioned_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(IsPartitionedable<I, C, P>() && IteratorRange<I, S>())>
            RANGES_CXX14_CONSTEXPR
            bool operator()(I begin, S end, C pred_, P proj_ = P{}) const
            {
                auto && pred = as_function(pred_);
                auto && proj = as_function(proj_);
                for(; begin != end; ++begin)
                    if(!pred(proj(*begin)))
                        break;
                for(; begin != end; ++begin)
                    if(pred(proj(*begin)))
                        return false;
                return true;
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsPartitionedable<I, C, P>() && Iterable<Rng>())>
            RANGES_CXX14_CONSTEXPR
            bool operator()(Rng &&rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `is_partitioned_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& is_partitioned = static_const<with_braced_init_args<is_partitioned_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
