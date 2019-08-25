/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_ALGORITHM_UNIQUE_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(unique)

        /// \brief template function \c unique
        ///
        /// range-based version of the \c unique std algorithm
        ///
        /// \pre `Rng` is a model of the `forward_range` concept
        /// \pre `I` is a model of the `forward_iterator` concept
        /// \pre `S` is a model of the `sentinel_for` concept
        /// \pre `C` is a model of the `relation` concept
        ///
        template<typename I, typename S, typename C = equal_to, typename P = identity>
        auto RANGES_FUN_NIEBLOID(unique)(I first, S last, C pred = C{}, P proj = P{}) //
            ->CPP_ret(I)(                                                             //
                requires sortable<I, C, P> && sentinel_for<S, I>)
        {
            first = adjacent_find(std::move(first), last, std::ref(pred), std::ref(proj));

            if(first != last)
            {
                for(I i = next(first); ++i != last;)
                    if(!invoke(pred, invoke(proj, *first), invoke(proj, *i)))
                        *++first = iter_move(i);
                ++first;
            }
            return first;
        }

        /// \overload
        template<typename Rng, typename C = equal_to, typename P = identity>
        auto RANGES_FUN_NIEBLOID(unique)(Rng && rng, C pred = C{}, P proj = P{}) //
            ->CPP_ret(safe_iterator_t<Rng>)(                                     //
                requires sortable<iterator_t<Rng>, C, P> && range<Rng>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(unique)

    namespace cpp20
    {
        using ranges::unique;
    }
    /// @}
} // namespace ranges

#endif // include guard
