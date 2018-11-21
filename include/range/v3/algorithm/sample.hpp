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
#ifndef RANGES_V3_ALGORITHM_SAMPLE_HPP
#define RANGES_V3_ALGORITHM_SAMPLE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/utility/random.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    inline namespace v3
    {
        CPP_def
        (
            template(typename I, typename S, typename O, typename Gen)
            concept SampleAlgoConcept,
                InputIterator<I> && Sentinel<S, I> && WeaklyIncrementable<O> &&
                IndirectlyCopyable<I, O> && UniformRandomNumberGenerator<Gen> &&
                ConvertibleTo<invoke_result_t<Gen &>, iter_difference_t<I>>
        );

        /// \addtogroup group-algorithms
        /// @{
        class sample_fn
        {
            template<typename I, typename S, typename O, typename Gen = detail::default_random_engine&>
            static auto sized_impl(I first, S last, iter_difference_t<I> pop_size, O out,
                    iter_difference_t<I> n, Gen &&gen) ->
                CPP_ret(tagged_pair<tag::in(I), tag::out(O)>)(
                    requires SampleAlgoConcept<I, S, O, Gen>)
            {
                std::uniform_int_distribution<iter_difference_t<I>> dist;
                using param_t = typename decltype(dist)::param_type;
                n = ranges::min(pop_size, n);
                for(; n > 0 && first != last; ++first)
                {
                    if(dist(gen, param_t{0, --pop_size}) < n)
                    {
                        --n;
                        *out = *first;
                        ++out;
                    }
                }
                return {std::move(first), std::move(out)};
            }
        public:
            template<typename I, typename S, typename O,
                typename Gen = detail::default_random_engine&>
            auto operator()(I first, S last, O out, iter_difference_t<I> n,
                    Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(I), tag::out(O)>)(
                    requires SampleAlgoConcept<I, S, O, Gen> &&
                        (ForwardIterator<I> || SizedSentinel<S, I>))
            {
                auto k = distance(first, last);
                return sample_fn::sized_impl(std::move(first), std::move(last),
                    k, std::move(out), n, static_cast<Gen &&>(gen));
            }
            template<typename I, typename S, typename O,
                typename Gen = detail::default_random_engine&>
            auto operator()(I first, S last, O out, iter_difference_t<I> n,
                Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(I), tag::out(O)>)(
                    requires RandomAccessIterator<O> && SampleAlgoConcept<I, S, O, Gen> &&
                        !(ForwardIterator<I> || SizedSentinel<S, I>) )
            {
                if(n <= 0)
                    goto done;
                for(iter_difference_t<I> i = 0; i < n; (void)++i, ++first)
                {
                    if(first == last)
                    {
                        out += i;
                        goto done;
                    }
                    out[i] = *first;
                }
                std::uniform_int_distribution<iter_difference_t<I>> dist;
                using param_t = typename decltype(dist)::param_type;
                for(auto pop_size = n; first != last; (void)++first, ++pop_size)
                {
                    auto const i = dist(gen, param_t{0, pop_size});
                    if(i < n)
                        out[i] = *first;
                }
                out += n;
            done:
                return {std::move(first), std::move(out)};
            }
            template<typename I, typename S, typename ORng,
                typename Gen = detail::default_random_engine&>
            auto operator()(I first, S last, ORng &&out,
                    Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(I), tag::out(safe_iterator_t<ORng>)>)(
                    requires SampleAlgoConcept<I, S, iterator_t<ORng>, Gen> &&
                        (ForwardIterator<I> || SizedSentinel<S, I>) &&
                        (ForwardRange<ORng> || SizedRange<ORng>))
            {
                auto k = distance(first, last);
                return sample_fn::sized_impl(std::move(first), std::move(last),
                    k, begin(out), distance(out), static_cast<Gen &&>(gen));
            }
            template<typename I, typename S, typename ORng,
                typename Gen = detail::default_random_engine&>
            auto operator()(I first, S last, ORng &&out,
                    Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(I), tag::out(safe_iterator_t<ORng>)>)(
                    requires RandomAccessIterator<iterator_t<ORng>> &&
                        SampleAlgoConcept<I, S, iterator_t<ORng>, Gen> &&
                        !(ForwardIterator<I> || SizedSentinel<S, I>) &&
                        (ForwardRange<ORng> || SizedRange<ORng>))
            {
                return (*this)(std::move(first), std::move(last), begin(out),
                    distance(out), static_cast<Gen &&>(gen));
            }
            template<typename Rng, typename O, typename Gen = detail::default_random_engine&>
            auto operator()(Rng &&rng, O out, range_difference_t<Rng> n,
                    Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>)(
                    requires RandomAccessIterator<O> &&
                        SampleAlgoConcept<iterator_t<Rng>, sentinel_t<Rng>, O, Gen> &&
                        !(ForwardRange<Rng> || SizedRange<Rng>))
            {
                return (*this)(begin(rng), end(rng),
                    std::move(out), n, static_cast<Gen &&>(gen));
            }
            template<typename Rng, typename O, typename Gen = detail::default_random_engine&>
            auto operator()(Rng &&rng, O out, range_difference_t<Rng> n,
                Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>)(
                    requires SampleAlgoConcept<iterator_t<Rng>, sentinel_t<Rng>, O, Gen> &&
                        (ForwardRange<Rng> || SizedRange<Rng>))
            {
                return sample_fn::sized_impl(begin(rng), end(rng), distance(rng),
                    std::move(out), n, static_cast<Gen &&>(gen));
            }
            template<typename IRng, typename ORng, typename Gen = detail::default_random_engine&>
            auto operator()(IRng &&rng, ORng &&out, Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(safe_iterator_t<IRng>), tag::out(safe_iterator_t<ORng>)>)(
                    requires RandomAccessIterator<iterator_t<ORng>> &&
                        SampleAlgoConcept<iterator_t<IRng>, sentinel_t<IRng>, iterator_t<ORng>, Gen> &&
                            !(ForwardRange<IRng> || SizedRange<IRng>) &&
                            (ForwardRange<ORng> || SizedRange<ORng>))
            {
                return (*this)(begin(rng), end(rng),
                    begin(out), distance(out), static_cast<Gen &&>(gen));
            }
            template<typename IRng, typename ORng, typename Gen = detail::default_random_engine&>
            auto operator()(IRng &&rng, ORng &&out, Gen &&gen = detail::get_random_engine()) const ->
                CPP_ret(tagged_pair<tag::in(safe_iterator_t<IRng>), tag::out(safe_iterator_t<ORng>)>)(
                    requires SampleAlgoConcept<iterator_t<IRng>, sentinel_t<IRng>, iterator_t<ORng>, Gen> &&
                        (ForwardRange<IRng> || SizedRange<IRng>) &&
                        (ForwardRange<ORng> || SizedRange<ORng>))
            {
                return sample_fn::sized_impl(begin(rng), end(rng), distance(rng),
                    begin(out), distance(out), static_cast<Gen &&>(gen));
            }
        };

        /// \sa `sample_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<sample_fn>, sample)
        /// @}
    } // namespace v3
} // namespace ranges

RANGES_RE_ENABLE_WARNINGS

#endif
