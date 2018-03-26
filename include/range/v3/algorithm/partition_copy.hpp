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
#ifndef RANGES_V3_ALGORITHM_PARTITION_COPY_HPP
#define RANGES_V3_ALGORITHM_PARTITION_COPY_HPP

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
        using PartitionCopyable = meta::strict_and<
            InputIterator<I>,
            WeaklyIncrementable<O0>,
            WeaklyIncrementable<O1>,
            IndirectlyCopyable<I, O0>,
            IndirectlyCopyable<I, O1>,
            IndirectPredicate<C, projected<I, P>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct partition_copy_fn
        {
            template<typename I, typename S, typename O0, typename O1, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionCopyable<I, O0, O1, C, P>() && Sentinel<S, I>())>
            tagged_tuple<tag::in(I), tag::out1(O0), tag::out2(O1)>
            operator()(I begin, S end, O0 o0, O1 o1, C pred, P proj = P{}) const
            {
                for(; begin != end; ++begin)
                {
                    auto &&x = *begin;
                    if(invoke(pred, invoke(proj, x)))
                    {
                        *o0 = (decltype(x) &&) x;
                        ++o0;
                    }
                    else
                    {
                        *o1 = (decltype(x) &&) x;
                        ++o1;
                    }
                }
                return make_tagged_tuple<tag::in, tag::out1, tag::out2>(begin, o0, o1);
            }

            template<typename Rng, typename O0, typename O1, typename C, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(PartitionCopyable<I, O0, O1, C, P>() && Range<Rng>())>
            tagged_tuple<tag::in(safe_iterator_t<Rng>), tag::out1(O0), tag::out2(O1)>
            operator()(Rng &&rng, O0 o0, O1 o1, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(o0), std::move(o1), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `partition_copy_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<partition_copy_fn>,
                               partition_copy)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
