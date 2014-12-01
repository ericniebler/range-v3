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
#ifndef RANGES_V3_ALGORITHM_TRANSFORM_HPP
#define RANGES_V3_ALGORITHM_TRANSFORM_HPP

#include <tuple>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O, typename F, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>,
            typename Y = concepts::Invokable::result_t<F, X>>
        using Transformable1 = meta::fast_and<
            InputIterator<I>,
            WeaklyIncrementable<O>,
            Invokable<P, V>,
            Invokable<F, X>,
            Writable<O, Y>>;

        /// \ingroup group-concepts
        template<typename I0, typename I1, typename O, typename F,
            typename P0 = ident, typename P1 = ident,
            typename V0 = iterator_value_t<I0>,
            typename X0 = concepts::Invokable::result_t<P0, V0>,
            typename V1 = iterator_value_t<I1>,
            typename X1 = concepts::Invokable::result_t<P1, V1>,
            typename Y = concepts::Invokable::result_t<F, X0, X1>>
        using Transformable2 = meta::fast_and<
            InputIterator<I0>,
            WeakInputIterator<I1>,
            WeaklyIncrementable<O>,
            Invokable<P0, V0>,
            Invokable<P1, V1>,
            Invokable<F, X0, X1>,
            Writable<O, Y>>;

        /// \addtogroup group-algorithms
        /// @{
        struct transform_fn
        {
            // Single-range variant
            template<typename I, typename S, typename O, typename F, typename P = ident,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && Transformable1<I, O, F, P>())>
            std::pair<I, O> operator()(I begin, S end, O out, F fun_, P proj_ = P{}) const
            {
                auto &&fun = invokable(fun_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin, ++out)
                    *out = fun(proj(*begin));
                return {begin, out};
            }

            template<typename Rng, typename O, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng &>() && Transformable1<I, O, F, P>())>
            std::pair<I, O> operator()(Rng & rng, O out, F fun, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(fun), std::move(proj));
            }

            // Double-range variant, 4-iterator version
            template<typename I0, typename S0, typename I1, typename S1, typename O, typename F,
                typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(IteratorRange<I0, S0>() && IteratorRange<I1, S1>() && Transformable2<I0, I1, O, F, P0, P1>())>
            std::tuple<I0, I1, O> operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, O out, F fun_,
                P0 proj0_ = P0{}, P1 proj1_ = P1{}) const
            {
                auto &&fun = invokable(fun_);
                auto &&proj0 = invokable(proj0_);
                auto &&proj1 = invokable(proj1_);
                for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1, ++out)
                    *out = fun(proj0(*begin0), proj1(*begin1));
                return std::tuple<I0, I1, O>{begin0, begin1, out};
            }

            template<typename Rng0, typename Rng1, typename O, typename F,
                typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = range_iterator_t<Rng1>,
                CONCEPT_REQUIRES_(Iterable<Rng0 &>() && Iterable<Rng1 &>() && Transformable2<I0, I1, O, F, P0, P1>())>
            std::tuple<I0, I1, O> operator()(Rng0 & rng0, Rng1 & rng1, O out, F fun,
                P0 proj0 = P0{}, P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), begin(rng1), end(rng1), std::move(out),
                    std::move(fun), std::move(proj0), std::move(proj1));
            }

            // Double-range variant, 3-iterator version
            template<typename I0, typename S0, typename I1, typename O, typename F,
                typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(IteratorRange<I0, S0>() && Transformable2<I0, I1, O, F, P0, P1>())>
            std::tuple<I0, I1, O> operator()(I0 begin0, S0 end0, I1 begin1, O out, F fun,
                P0 proj0 = P0{}, P1 proj1 = P1{}) const
            {
                return (*this)(std::move(begin0), std::move(end0), std::move(begin1), unreachable{},
                    std::move(out), std::move(fun), std::move(proj0), std::move(proj1));
            }

            template<typename Rng0, typename I1Ref, typename O, typename F,
                typename P0 = ident, typename P1 = ident, typename I1 = uncvref_t<I1Ref>,
                typename I0 = range_iterator_t<Rng0>,
                CONCEPT_REQUIRES_(Iterable<Rng0 &>() && Transformable2<I0, I1, O, F, P0, P1>())>
            std::tuple<I0, I1, O> operator()(Rng0 & rng0, I1Ref &&begin1, O out, F fun,
                P0 proj0 = P0{}, P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), std::forward<I1Ref>(begin1), unreachable{},
                    std::move(out), std::move(fun), std::move(proj0), std::move(proj1));
            }
        };

        /// \sa `transform_fn`
        /// \ingroup group-algorithms
        constexpr transform_fn transform{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
