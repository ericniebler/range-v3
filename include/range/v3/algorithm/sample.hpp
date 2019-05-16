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
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/copy_n.hpp>
#include <range/v3/utility/random.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using sample_result = detail::in_out_result<I, O>;

    struct sample_fn
    {
    private:
        template<typename I, typename S, typename O, typename Gen>
        static auto sized_impl(I first, S last, iter_difference_t<I> pop_size, O out,
                iter_difference_t<O> sample_size, Gen &&gen) ->
            sample_result<I, O>
        {
            if(pop_size > 0 && sample_size > 0)
            {
                std::uniform_int_distribution<difference_type_t<I>> dist;
                using param_t = typename decltype(dist)::param_type;
                for(; first != last; ++first)
                {
                    if(sample_size >= pop_size)
                        return copy_n(std::move(first), pop_size, std::move(out));

                    if(dist(gen, param_t{0, --pop_size}) < sample_size)
                    {
                        *out = *first;
                        ++out;
                        if(--sample_size == 0)
                            break;
                    }
                }
            }

            return {std::move(first), std::move(out)};
        }

    public:
        template<typename I, typename S, typename O,
            typename Gen = detail::default_random_engine &>
        auto operator()(I first, S last, O out, iter_difference_t<O> const n,
                Gen &&gen = detail::get_random_engine()) const ->
            CPP_ret(sample_result<I, O>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    WeaklyIncrementable<O> &&
                    IndirectlyCopyable<I, O> &&
                    UniformRandomNumberGenerator<Gen> &&
                    (RandomAccessIterator<O> || ForwardIterator<I> || SizedSentinel<S, I>))
        {
            if RANGES_CONSTEXPR_IF (ForwardIterator<I> || SizedSentinel<S, I>)
            {
                auto const k = distance(first, last);
                return sample_fn::sized_impl(std::move(first), std::move(last),
                    k, std::move(out), n, static_cast<Gen &&>(gen));
            }
            else
            {
                // out is random-access here; calls to advance(out,n) and
                // next(out,n) are O(1).
                if(n > 0)
                {
                    for(difference_type_t<O> i = 0; i < n; (void)++i, ++first)
                    {
                        if(first == last)
                        {
                            advance(out, i);
                            goto done;
                        }
                        *next(out, i) = *first;
                    }

                    std::uniform_int_distribution<difference_type_t<O>> dist;
                    using param_t = typename decltype(dist)::param_type;
                    for(auto pop_size = n; first != last; (void)++first, ++pop_size)
                    {
                        auto const i = dist(gen, param_t{0, pop_size});
                        if(i < n)
                            *next(out, i) = *first;
                    }

                    advance(out, n);
                }
            done:
                return {std::move(first), std::move(out)};
            }
        }

        template<typename I, typename S, typename ORng,
            typename Gen = detail::default_random_engine &>
        auto operator()(I first, S last, ORng &&out,
                Gen &&gen = detail::get_random_engine()) const ->
            CPP_ret(sample_result<I, safe_iterator_t<ORng>>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    WeaklyIncrementable<iterator_t<ORng>> &&
                    IndirectlyCopyable<I, iterator_t<ORng>> &&
                    UniformRandomNumberGenerator<Gen> &&
                    (ForwardRange<ORng> || SizedRange<ORng>) &&
                    (RandomAccessIterator<iterator_t<ORng>> || ForwardIterator<I> || SizedSentinel<S, I>))
        {
            if RANGES_CONSTEXPR_IF (ForwardIterator<I> || SizedSentinel<S, I>)
            {
                auto k = distance(first, last);
                return sample_fn::sized_impl(std::move(first), std::move(last),
                    k, begin(out), distance(out), static_cast<Gen &&>(gen));
            }
            else
            {
                return (*this)(std::move(first), std::move(last), begin(out),
                    distance(out), static_cast<Gen &&>(gen));
            }
        }

        template<typename Rng, typename O, typename Gen = detail::default_random_engine &>
        auto operator()(Rng &&rng, O out, difference_type_t<O> const n,
            Gen &&gen = detail::get_random_engine()) const ->
            CPP_ret(sample_result<safe_iterator_t<Rng>, O>)(
                requires InputRange<Rng> &&
                    WeaklyIncrementable<O> &&
                    IndirectlyCopyable<iterator_t<Rng>, O> &&
                    UniformRandomNumberGenerator<Gen> &&
                    (RandomAccessIterator<O> || ForwardRange<Rng> || SizedRange<Rng>))
        {
            if RANGES_CONSTEXPR_IF (ForwardRange<Rng> || SizedRange<Rng>)
            {
                return sample_fn::sized_impl(begin(rng), end(rng), distance(rng),
                    std::move(out), n, static_cast<Gen &&>(gen));
            }
            else
            {
                return (*this)(begin(rng), end(rng),
                    std::move(out), n, static_cast<Gen &&>(gen));
            }
        }

        template<typename IRng, typename ORng, typename Gen = detail::default_random_engine &>
        auto operator()(IRng &&rng, ORng &&out, Gen &&gen = detail::get_random_engine()) const ->
            CPP_ret(sample_result<safe_iterator_t<IRng>, safe_iterator_t<ORng>>)(
                requires InputRange<IRng> && Range<ORng> &&
                    IndirectlyCopyable<iterator_t<IRng>, iterator_t<ORng>> &&
                    UniformRandomNumberGenerator<Gen> &&
                    (RandomAccessIterator<iterator_t<ORng>> || ForwardRange<IRng> || SizedRange<IRng>) &&
                    (ForwardRange<ORng> || SizedRange<ORng>))
        {
            if RANGES_CONSTEXPR_IF (ForwardRange<IRng> || SizedRange<IRng>)
            {
                return sample_fn::sized_impl(begin(rng), end(rng), distance(rng),
                    begin(out), distance(out), static_cast<Gen &&>(gen));
            }
            else
            {
                return (*this)(begin(rng), end(rng),
                    begin(out), distance(out), static_cast<Gen &&>(gen));
            }
        }
    };

    /// \sa `sample_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(sample_fn, sample)

    // Not yet!
    //  namespace cpp20
    // {
    //     using ranges::sample_result;
    //     using ranges::sample;
    // }
    /// @}
} // namespace ranges

RANGES_RE_ENABLE_WARNINGS

#endif
