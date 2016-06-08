/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_SAMPLE_HPP
#define RANGES_V3_ALGORITHM_SAMPLE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/random.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        class sample_fn
        {
            template<typename I, typename S, typename O, typename Gen>
            using Constraint = meta::strict_and<
                InputIterator<I>, IteratorRange<I, S>, WeaklyIncrementable<O>,
                IndirectlyCopyable<I, O>, UniformRandomNumberGenerator<Gen>,
                ConvertibleTo<concepts::UniformRandomNumberGenerator::result_t<Gen>,
                    iterator_difference_t<I>>>;

            template<typename I, typename S, typename O,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(Constraint<I, S, O, Gen>())>
            static O sized_impl(I first, S last, iterator_difference_t<I> pop_size,
                O out, iterator_difference_t<I> n, Gen && gen)
            {
                std::uniform_int_distribution<iterator_difference_t<I>> dist;
                using param_t = typename decltype(dist)::param_type;
                n = std::min(pop_size, n);
                for (; n > 0 && first != last; ++first)
                {
                    if (dist(gen, param_t{0, --pop_size}) < n)
                    {
                        --n;
                        *out = *first;
                        ++out;
                    }
                }
                return out;
            }
        public:
            template<typename I, typename S, typename O,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(
                    (ForwardIterator<I>() || SizedIteratorRange<I, S>()) &&
                    Constraint<I, S, O, Gen>())>
            O operator()(I first, S last, O out, iterator_difference_t<I> n,
                Gen && gen = detail::get_random_engine()) const
            {
                auto k = distance(first, last);
                return sample_fn::sized_impl(std::move(first), std::move(last),
                    k, out, n, std::forward<Gen>(gen));
            }
            template<typename I, typename S, typename O,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(RandomAccessIterator<O>() &&
                    !(ForwardIterator<I>() || SizedIteratorRange<I, S>()) &&
                    Constraint<I, S, O, Gen>())>
            O operator()(I first, S last, O out, iterator_difference_t<I> n,
                Gen && gen = detail::get_random_engine()) const
            {
                if (n > 0)
                {
                    for (auto i = n - n; i < n && first != last; (void)++i, ++first)
                    {
                        out[i] = *first;
                    }
                    std::uniform_int_distribution<iterator_difference_t<I>> dist;
                    using param_t = typename decltype(dist)::param_type;
                    for (auto pop_size = n; first != last; (void)++first, ++pop_size)
                    {
                        auto const i = dist(gen, param_t{0, pop_size});
                        if (i < n)
                            out[i] = *first;
                    }
                    out += n;
                }
                return out;
            }
            template<typename I, typename S, typename ORng,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(
                    (ForwardIterator<I>() || SizedIteratorRange<I, S>()) &&
                    (ForwardRange<ORng>() || SizedRange<ORng>()) &&
                    Constraint<I, S, range_iterator_t<ORng>, Gen>())>
            range_safe_iterator_t<ORng> operator()(I first, S last, ORng && out,
                Gen && gen = detail::get_random_engine()) const
            {
                auto k = distance(first, last);
                return sample_fn::sized_impl(std::move(first), std::move(last),
                    k, begin(out), distance(out), std::forward<Gen>(gen));
            }
            template<typename I, typename S, typename ORng,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(RandomAccessIterator<range_iterator_t<ORng>>() &&
                    !(ForwardIterator<I>() || SizedIteratorRange<I, S>()) &&
                    (ForwardRange<ORng>() || SizedRange<ORng>()) &&
                    Constraint<I, S, range_iterator_t<ORng>, Gen>())>
            range_safe_iterator_t<ORng> operator()(I first, S last, ORng && out,
                Gen && gen = detail::get_random_engine()) const
            {
                return (*this)(std::move(first), std::move(last), begin(out),
                    distance(out), std::forward<Gen>(gen));
            }
            template<typename Rng, typename O,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(RandomAccessIterator<O>() &&
                    !(ForwardRange<Rng>() || SizedRange<Rng>()) &&
                    Constraint<range_iterator_t<Rng>, range_sentinel_t<Rng>, O, Gen>())>
            O operator()(Rng && rng, O out, range_difference_t<Rng> n,
                Gen && gen = detail::get_random_engine()) const
            {
                return (*this)(begin(rng), end(rng),
                    std::move(out), n, std::forward<Gen>(gen));
            }
            template<typename Rng, typename O,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(
                    (ForwardRange<Rng>() || SizedRange<Rng>()) &&
                    Constraint<range_iterator_t<Rng>, range_sentinel_t<Rng>, O, Gen>())>
            O operator()(Rng && rng, O out, range_difference_t<Rng> n,
                Gen && gen = detail::get_random_engine()) const
            {
                return sample_fn::sized_impl(begin(rng), end(rng), distance(rng),
                    std::move(out), n, std::forward<Gen>(gen));
            }
            template<typename IRng, typename ORng,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(RandomAccessIterator<range_iterator_t<ORng>>() &&
                    !(ForwardRange<IRng>() || SizedRange<IRng>()) &&
                    (ForwardRange<ORng>() || SizedRange<ORng>()) &&
                    Constraint<range_iterator_t<IRng>, range_sentinel_t<IRng>,
                        range_iterator_t<ORng>, Gen>())>
            range_safe_iterator_t<ORng> operator()(IRng && rng, ORng && out,
                Gen && gen = detail::get_random_engine()) const
            {
                return (*this)(begin(rng), end(rng),
                    begin(out), distance(out), std::forward<Gen>(gen));
            }
            template<typename IRng, typename ORng,
                typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(
                    (ForwardRange<IRng>() || SizedRange<IRng>()) &&
                    (ForwardRange<ORng>() || SizedRange<ORng>()) &&
                    Constraint<range_iterator_t<IRng>, range_sentinel_t<IRng>,
                        range_iterator_t<ORng>, Gen>())>
            range_safe_iterator_t<ORng> operator()(IRng && rng, ORng && out,
                Gen && gen = detail::get_random_engine()) const
            {
                return sample_fn::sized_impl(begin(rng), end(rng), distance(rng),
                    begin(out), distance(out), std::forward<Gen>(gen));
            }
        };

        /// \sa `sample_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& sample = static_const<with_braced_init_args<sample_fn>>::value;
        }
        /// @}
    } // namespace v3
} // namespace ranges

#endif
