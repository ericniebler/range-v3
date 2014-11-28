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
#ifndef RANGES_V3_ALGORITHM_PARTITION_COPY_HPP
#define RANGES_V3_ALGORITHM_PARTITION_COPY_HPP

#include <tuple>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename O0, typename O1, typename C, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using PartitionCopyable = meta::fast_and<
            InputIterator<I>,
            WeaklyIncrementable<O0>,
            WeaklyIncrementable<O1>,
            IndirectlyCopyable<I, O0>,
            IndirectlyCopyable<I, O1>,
            Invokable<P, V>,
            InvokablePredicate<C, X>>;

        struct partition_copy_fn
        {
            template<typename I, typename S, typename O0, typename O1, typename C, typename P = ident,
                CONCEPT_REQUIRES_(PartitionCopyable<I, O0, O1, C, P>() && IteratorRange<I, S>())>
            std::tuple<I, O0, O1> operator()(I begin, S end, O0 o0, O1 o1, C pred_, P proj_ = P{}) const
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
                for(; begin != end; ++begin)
                {
                    if(pred(proj(*begin)))
                    {
                        *o0 = *begin;
                        ++o0;
                    }
                    else
                    {
                        *o1 = *begin;
                        ++o1;
                    }
                }
                return std::tuple<I, O0, O1>{begin, o0, o1};
            }

            template<typename Rng, typename O0, typename O1, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(PartitionCopyable<I, O0, O1, C, P>() && Iterable<Rng &>())>
            std::tuple<I, O0, O1> operator()(Rng &rng, O0 o0, O1 o1, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(o0), std::move(o1), std::move(pred),
                    std::move(proj));
            }
        };

        constexpr partition_copy_fn partition_copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
