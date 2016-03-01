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
#ifndef RANGES_V3_ALGORITHM_PARTITION_MOVE_HPP
#define RANGES_V3_ALGORITHM_PARTITION_MOVE_HPP

#include <tuple>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_tuple.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O0, typename O1, typename C, typename P = ident>
        using PartitionMovable = meta::strict_and<
            InputIterator<I>,
            WeaklyIncrementable<O0>,
            WeaklyIncrementable<O1>,
            IndirectlyMovable<I, O0>,
            IndirectlyMovable<I, O1>,
            IndirectCallablePredicate<C, Projected<I, P>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct partition_move_fn
        {
            template<typename I, typename S, typename O0, typename O1, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionMovable<I, O0, O1, C, P>() && IteratorRange<I, S>())>
            tagged_tuple<tag::in(I), tag::out1(O0), tag::out2(O1)> operator()(I begin, S end, O0 o0, O1 o1, C pred_, P proj_ = P{}) const
            {
                auto && pred = as_function(pred_);
                auto && proj = as_function(proj_);
                for(; begin != end; ++begin)
                {
                    if(pred(proj(*begin)))
                    {
                        *o0 = iter_move(begin);
                        ++o0;
                    }
                    else
                    {
                        *o1 = iter_move(begin);
                        ++o1;
                    }
                }
                return make_tagged_tuple<tag::in, tag::out1, tag::out2>(begin, o0, o1);
            }

            template<typename Rng, typename O0, typename O1, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(PartitionMovable<I, O0, O1, C, P>() && Range<Rng>())>
            tagged_tuple<tag::in(range_safe_iterator_t<Rng>), tag::out1(O0), tag::out2(O1)>
            operator()(Rng &&rng, O0 o0, O1 o1, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(o0), std::move(o1), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `partition_move_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& partition_move = static_const<with_braced_init_args<partition_move_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
