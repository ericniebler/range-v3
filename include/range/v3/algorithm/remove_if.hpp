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
#ifndef RANGES_V3_ALGORITHM_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct remove_if_fn
    {
        template<typename I, typename S, typename C, typename P = identity>
        auto operator()(I begin, S end, C pred, P proj = P{}) const ->
            CPP_ret(I)(
                requires Permutable<I> && Sentinel<S, I> &&
                    IndirectUnaryPredicate<C, projected<I, P>>)
        {
            begin = find_if(std::move(begin), end, std::ref(pred), std::ref(proj));
            if(begin != end)
            {
                for(I i = next(begin); i != end; ++i)
                {
                    if(!(invoke(pred, invoke(proj, *i))))
                    {
                        *begin = iter_move(i);
                        ++begin;
                    }
                }
            }
            return begin;
        }

        template<typename Rng, typename C, typename P = identity>
        auto operator()(Rng &&rng, C pred, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires ForwardRange<Rng> && Permutable<iterator_t<Rng>> &&
                    IndirectUnaryPredicate<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `remove_if_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(remove_if_fn, remove_if)
    /// @}
} // namespace ranges

#endif // include guard
