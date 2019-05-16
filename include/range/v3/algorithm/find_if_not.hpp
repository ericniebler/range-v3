/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FIND_IF_NOT_HPP
#define RANGES_V3_ALGORITHM_FIND_IF_NOT_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct find_if_not_fn
    {
        /// \brief template function \c find_if_not_fn::operator()
        ///
        /// range-based version of the \c find_if_not std algorithm
        ///
        /// \pre `Rng` is a model of the `Range` concept
        /// \pre `I` is a model of the `InputIterator` concept
        /// \pre `S` is a model of the `Sentinel<I>` concept
        /// \pre `P` is a model of the `Invocable<V>` concept, where `V` is the
        ///      value type of I.
        /// \pre `F` models `Predicate<X>`, where `X` is the result type
        ///      of `Invocable<P, V>`
        template<typename I, typename S, typename F, typename P = identity>
        auto operator()(I begin, S end, F pred, P proj = P{}) const ->
            CPP_ret(I)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    IndirectUnaryPredicate<F, projected<I, P>>)
        {
            for(; begin != end; ++begin)
                if(!invoke(pred, invoke(proj, *begin)))
                    break;
            return begin;
        }

        /// \overload
        template<typename Rng, typename F, typename P = identity>
        auto operator()(Rng &&rng, F pred, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires InputRange<Rng> && IndirectUnaryPredicate<F, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `find_if_not_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(find_if_not_fn, find_if_not)

    namespace cpp20
    {
        using ranges::find_if_not;
    }
    /// @}
} // namespace ranges

#endif // include guard
