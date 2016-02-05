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
#ifndef RANGES_V3_ALGORITHM_TRANSFORM_HPP
#define RANGES_V3_ALGORITHM_TRANSFORM_HPP

#include <tuple>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/utility/tagged_tuple.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O, typename F, typename P = ident,
            typename V = iterator_common_reference_t<I>,
            typename X = concepts::Callable::result_t<P, V>,
            typename Y = concepts::Callable::result_t<F, X>>
        using Transformable1 = meta::fast_and<
            InputIterator<I>,
            WeaklyIncrementable<O>,
            IndirectCallable<F, Projected<I, P>>,
            Writable<O, Y &&>>;

        /// \ingroup group-concepts
        template<typename I0, typename I1, typename O, typename F,
            typename P0 = ident, typename P1 = ident,
            typename V0 = iterator_common_reference_t<I0>,
            typename X0 = concepts::Callable::result_t<P0, V0>,
            typename V1 = iterator_common_reference_t<I1>,
            typename X1 = concepts::Callable::result_t<P1, V1>,
            typename Y = concepts::Callable::result_t<F, X0, X1>>
        using Transformable2 = meta::fast_and<
            InputIterator<I0>,
            InputIterator<I1>,
            WeaklyIncrementable<O>,
            IndirectCallable<F, Projected<I0, P0>, Projected<I1, P1>>,
            Writable<O, Y &&>>;

        /// \addtogroup group-algorithms
        /// @{
        struct transform_fn
        {
            // Single-range variant
            template<typename I, typename S, typename O, typename F, typename P = ident,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && Transformable1<I, O, F, P>())>
            tagged_pair<tag::in(I), tag::out(O)> operator()(I begin, S end, O out, F fun_, P proj_ = P{}) const
            {
                auto &&fun = as_function(fun_);
                auto &&proj = as_function(proj_);
                for(; begin != end; ++begin, ++out)
                    *out = fun(proj(*begin));
                return {begin, out};
            }

            template<typename Rng, typename O, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Range<Rng>() && Transformable1<I, O, F, P>())>
            tagged_pair<tag::in(range_safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, O out, F fun, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(fun), std::move(proj));
            }

            // Double-range variant, 4-iterator version
            template<typename I0, typename S0, typename I1, typename S1, typename O, typename F,
                typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(IteratorRange<I0, S0>() && IteratorRange<I1, S1>() &&
                    Transformable2<I0, I1, O, F, P0, P1>())>
            tagged_tuple<tag::in1(I0), tag::in2(I1), tag::out(O)> operator()(I0 begin0, S0 end0, I1 begin1, S1 end1, O out, F fun_,
                P0 proj0_ = P0{}, P1 proj1_ = P1{}) const
            {
                auto &&fun = as_function(fun_);
                auto &&proj0 = as_function(proj0_);
                auto &&proj1 = as_function(proj1_);
                for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1, ++out)
                    *out = fun(proj0(*begin0), proj1(*begin1));
                return tagged_tuple<tag::in1(I0), tag::in2(I1), tag::out(O)>{begin0, begin1, out};
            }

            template<typename Rng0, typename Rng1, typename O, typename F,
                typename P0 = ident, typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = range_iterator_t<Rng1>,
                CONCEPT_REQUIRES_(Range<Rng0>() && Range<Rng1>() &&
                    Transformable2<I0, I1, O, F, P0, P1>())>
            tagged_tuple<tag::in1(range_safe_iterator_t<Rng0>), tag::in2(range_safe_iterator_t<Rng1>), tag::out(O)>
            operator()(Rng0 &&rng0, Rng1 &&rng1, O out, F fun, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), begin(rng1), end(rng1), std::move(out),
                    std::move(fun), std::move(proj0), std::move(proj1));
            }

            // Double-range variant, 3-iterator version
            template<typename I0, typename S0, typename I1, typename O, typename F,
                typename P0 = ident, typename P1 = ident,
                CONCEPT_REQUIRES_(IteratorRange<I0, S0>() &&
                    Transformable2<I0, I1, O, F, P0, P1>())>
            tagged_tuple<tag::in1(I0), tag::in2(I1), tag::out(O)>
            operator()(I0 begin0, S0 end0, I1 begin1, O out, F fun, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(std::move(begin0), std::move(end0), std::move(begin1), unreachable{},
                    std::move(out), std::move(fun), std::move(proj0), std::move(proj1));
            }

            template<typename Rng0, typename I1Ref, typename O, typename F,
                typename P0 = ident, typename P1 = ident, typename I1 = uncvref_t<I1Ref>,
                typename I0 = range_iterator_t<Rng0>,
                CONCEPT_REQUIRES_(Range<Rng0>() && Iterator<I1>() &&
                    Transformable2<I0, I1, O, F, P0, P1>())>
            tagged_tuple<tag::in1(range_safe_iterator_t<Rng0>), tag::in2(I1), tag::out(O)>
            operator()(Rng0 &&rng0, I1Ref &&begin1, O out, F fun, P0 proj0 = P0{},
                P1 proj1 = P1{}) const
            {
                return (*this)(begin(rng0), end(rng0), std::forward<I1Ref>(begin1), unreachable{},
                    std::move(out), std::move(fun), std::move(proj0), std::move(proj1));
            }
        };

        /// \sa `transform_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& transform = static_const<with_braced_init_args<transform_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
