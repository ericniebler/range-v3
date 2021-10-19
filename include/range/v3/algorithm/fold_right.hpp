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

#ifndef RANGES_V3_ALGORITHM_FOLD_RIGHT_HPP
#define RANGES_V3_ALGORITHM_FOLD_RIGHT_HPP

#include <meta/meta.hpp>

#include <range/v3/algorithm/fold_left.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/optional.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    namespace detail
    {
        template<typename F>
        struct flipped
        {
            F f;

            template(class T, class U)(requires invocable<F &, U, T>) //
                invoke_result_t<F &, U, T>
                operator()(T &&, U &&);
        };
    } // namespace detail

    // clang-format off
    template <class F, class T, class I>
    CPP_concept indirectly_binary_right_foldable =
        indirectly_binary_left_foldable<detail::flipped<F>, T, I>;
    // clang-format on

    /// \addtogroup group-algorithms
    /// @{
    struct fold_right_fn
    {
        template(typename I, typename S, typename T, typename Op)(
            /// \pre
            requires sentinel_for<S, I> AND bidirectional_iterator<I> AND
                indirectly_binary_right_foldable<Op, T, I>) //
            constexpr auto
            operator()(I first, S last, T init, Op op) const
        {
            using U = std::decay_t<invoke_result_t<Op &, iter_reference_t<I>, T>>;

            if(first == last)
            {
                return U(std::move(init));
            }

            I tail = next(first, last);
            U accum = invoke(op, *--tail, std::move(init));
            while(first != tail)
            {
                accum = invoke(op, *--tail, std::move(accum));
            }
            return accum;
        }

        template(typename Rng, typename T, typename Op)(
            /// \pre
            requires bidirectional_range<Rng> AND
                indirectly_binary_right_foldable<Op, T, iterator_t<Rng>>) //
            constexpr auto
            operator()(Rng && rng, T init, Op op) const
        {
            return (*this)(begin(rng), end(rng), std::move(init), std::move(op));
        }
    };

    struct fold_right_last_fn
    {
        template(typename I, typename S, typename Op)(
            /// \pre
            requires sentinel_for<S, I> AND bidirectional_iterator<I> AND
                indirectly_binary_right_foldable<Op, iter_value_t<I>, I>
                    AND constructible_from<iter_value_t<I>, iter_reference_t<I>>) //
            constexpr auto
            operator()(I first, S last, Op op) const
        {
            using U = invoke_result_t<fold_right_fn, I, S, iter_value_t<I>, Op>;
            if(first == last)
            {
                return optional<U>();
            }

            I tail = prev(next(first, std::move(last)));
            return optional<U>(
                in_place,
                fold_right_fn{}(
                    std::move(first), tail, iter_value_t<I>(*tail), std::move(op)));
        }

        template(typename R, typename Op)(
            /// \pre
            requires bidirectional_range<R> AND
                indirectly_binary_right_foldable<Op, range_value_t<R>, iterator_t<R>>
                    AND constructible_from<range_value_t<R>, range_reference_t<R>>) //
            constexpr auto
            operator()(R && rng, Op op) const
        {
            return (*this)(begin(rng), end(rng), std::move(op));
        }
    };

    RANGES_INLINE_VARIABLE(fold_right_fn, fold_right)
    RANGES_INLINE_VARIABLE(fold_right_last_fn, fold_right_last)
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
