/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct min_element_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires ForwardIterator<I> && Sentinel<S, I> &&
                    IndirectStrictWeakOrder<C, projected<I, P>>)
        {
            if(begin != end)
                for(auto tmp = next(begin); tmp != end; ++tmp)
                    if(invoke(pred, invoke(proj, *tmp), invoke(proj, *begin)))
                        begin = tmp;
            return begin;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires ForwardRange<Rng> &&
                    IndirectStrictWeakOrder<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `min_element_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(min_element_fn, min_element)

    namespace cpp20
    {
        using ranges::min_element;
    }
   /// @}
} // namespace ranges

#endif // include guard
