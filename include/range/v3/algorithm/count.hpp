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
#ifndef RANGES_V3_ALGORITHM_COUNT_HPP
#define RANGES_V3_ALGORITHM_COUNT_HPP

#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(count)

        /// \brief function template \c count
        template<typename I, typename S, typename V, typename P = identity>
        auto RANGES_FUN_NIEBLOID(count)(I first, S last, V const & val, P proj = P{})
            ->CPP_ret(iter_difference_t<I>)( //
                requires input_iterator<I> && sentinel_for<S, I> &&
                indirect_relation<equal_to, projected<I, P>, V const *>)
        {
            iter_difference_t<I> n = 0;
            for(; first != last; ++first)
                if(invoke(proj, *first) == val)
                    ++n;
            return n;
        }

        /// \overload
        template<typename Rng, typename V, typename P = identity>
        auto RANGES_FUN_NIEBLOID(count)(Rng && rng, V const & val, P proj = P{})
            ->CPP_ret(iter_difference_t<iterator_t<Rng>>)( //
                requires input_range<Rng> &&
                indirect_relation<equal_to, projected<iterator_t<Rng>, P>, V const *>)
        {
            return (*this)(begin(rng), end(rng), val, std::move(proj));
        }

    RANGES_END_NIEBLOID(count)

    namespace cpp20
    {
        using ranges::count;
    }
    /// @}
} // namespace ranges

#endif // include guard
