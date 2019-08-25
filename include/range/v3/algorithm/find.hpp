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
#ifndef RANGES_V3_ALGORITHM_FIND_HPP
#define RANGES_V3_ALGORITHM_FIND_HPP

#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(find)
        /// \brief template function \c find
        ///
        /// range-based version of the \c find std algorithm
        ///
        /// \pre `Rng` is a model of the `Range` concept
        /// \pre `I` is a model of the `input_iterator` concept
        /// \pre `S` is a model of the `sentinel_for<I>` concept
        /// \pre `P` is a model of the `invocable<iter_common_reference_t<I>>` concept
        /// \pre The ResultType of `P` is equality_comparable with V
        template<typename I, typename S, typename V, typename P = identity>
        auto RANGES_FUN_NIEBLOID(find)(I first, S last, V const & val, P proj = P{})
            ->CPP_ret(I)( //
                requires input_iterator<I> && sentinel_for<S, I> &&
                indirect_relation<equal_to, projected<I, P>, V const *>)
        {
            for(; first != last; ++first)
                if(invoke(proj, *first) == val)
                    break;
            return first;
        }

        /// \overload
        template<typename Rng, typename V, typename P = identity>
        auto RANGES_FUN_NIEBLOID(find)(Rng && rng, V const & val, P proj = P{})
            ->CPP_ret(safe_iterator_t<Rng>)( //
                requires input_range<Rng> &&
                indirect_relation<equal_to, projected<iterator_t<Rng>, P>, V const *>)
        {
            return (*this)(begin(rng), end(rng), val, std::move(proj));
        }

    RANGES_END_NIEBLOID(find)

    namespace cpp20
    {
        using ranges::find;
    }
    /// @}
} // namespace ranges

#endif // include guard
