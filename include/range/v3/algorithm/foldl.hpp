/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2021-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef RANGES_V3_ALGORITHM_FOLDL_HPP
#define RANGES_V3_ALGORITHM_FOLDL_HPP

#include <meta/meta.hpp>

#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    // clang-format off
    template<class F, class T, class I, class U>
    CPP_concept indirectly_binary_left_foldable_impl =
        movable<T> &&
        movable<U> &&
        convertible_to<T, U> &&
        invocable<F&, U, iter_reference_t<I>> &&
        assignable_from<U&, invoke_result_t<F&, U, iter_reference_t<I>>>;

    template <class F, class T, class I>
    CPP_concept indirectly_binary_left_foldable =
        copy_constructible<F> &&
        indirectly_readable<I> &&
        invocable<F&, T, iter_reference_t<I>> &&
        convertible_to<invoke_result_t<F&, T, iter_reference_t<I>>,
          std::decay_t<invoke_result_t<F&, T, iter_reference_t<I>>>> &&
        indirectly_binary_left_foldable_impl<F, T, I, std::decay_t<invoke_result_t<F&, T, iter_reference_t<I>>>>;
    // clang-format on

    /// \addtogroup group-algorithms
    /// @{
    struct foldl_fn
    {
        template(typename I, typename S, typename T, typename Op, typename P = identity)(
            /// \pre
            requires sentinel_for<S, I> AND input_iterator<I> AND
                indirectly_binary_left_foldable<Op, T, projected<I, P>>) //
            constexpr auto
            operator()(I first, S last, T init, Op op, P proj = P{}) const
        {
            using U = std::decay_t<invoke_result_t<Op &, T, iter_reference_t<I>>>;

            if(first == last)
            {
                return U(std::move(init));
            }

            U accum = invoke(op, std::move(init), *first);
            for(++first; first != last; ++first)
            {
                accum = invoke(op, std::move(accum), invoke(proj, *first));
            }
            return accum;
        }

        template(typename Rng, typename T, typename Op, typename P = identity)(
            /// \pre
            requires input_range<Rng> AND
                indirectly_binary_left_foldable<Op, T, projected<iterator_t<Rng>, P>>) //
            constexpr auto
            operator()(Rng && rng, T init, Op op, P proj = P{}) const
        {
            return (*this)(begin(rng),
                           end(rng),
                           std::move(init),
                           std::move(op),
                           std::move(proj));
        }
    };

    RANGES_INLINE_VARIABLE(foldl_fn, foldl)
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
