/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_MIN_HPP
#define RANGES_V3_ALGORITHM_MIN_HPP

#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct min_fn
    {
        CPP_template(typename T, typename C = less, typename P = identity)(
            requires IndirectStrictWeakOrder<C, projected<T const *, P>>)
        constexpr T const & operator()(T const &a, T const &b, C pred = C{}, P proj = P{}) const
        {
            return invoke(pred, invoke(proj, b), invoke(proj, a)) ? b : a;
        }

        CPP_template(typename Rng, typename C = less, typename P = identity)(
            requires InputRange<Rng> &&
                IndirectStrictWeakOrder<C, projected<iterator_t<Rng>, P>> &&
                IndirectlyCopyableStorable<iterator_t<Rng>, range_value_t<Rng> *>)
        constexpr /*c++14*/ range_value_t<Rng> operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
        {
            auto begin = ranges::begin(rng);
            auto end = ranges::end(rng);
            RANGES_EXPECT(begin != end);
            range_value_t<Rng> result = *begin;
            while(++begin != end)
            {
                auto &&tmp = *begin;
                if(invoke(pred, invoke(proj, tmp), invoke(proj, result)))
                    result = (decltype(tmp) &&) tmp;
            }
            return result;
        }

        CPP_template(typename T, typename C = less, typename P = identity)(
            requires Copyable<T> && IndirectStrictWeakOrder<C, projected<T const *, P>>)
        constexpr /*c++14*/ T operator()(std::initializer_list<T> const &&rng, C pred = C{},
                P proj = P{}) const
        {
            return (*this)(rng, std::move(pred), std::move(proj));
        }
    };

    /// \sa `min_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(min_fn, min)
    /// @}
} // namespace ranges

#endif // include guard
