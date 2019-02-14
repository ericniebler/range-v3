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
    struct find_fn
    {
        /// \brief template function \c find_fn::operator()
        ///
        /// range-based version of the \c find std algorithm
        ///
        /// \pre `Rng` is a model of the `Range` concept
        /// \pre `I` is a model of the `InputIterator` concept
        /// \pre `S` is a model of the `Sentinel<I>` concept
        /// \pre `P` is a model of the `Invocable<iter_common_reference_t<I>>` concept
        /// \pre The ResultType of `P` is EqualityComparable with V
        CPP_template(typename I, typename S, typename V, typename P = identity)(
            requires InputIterator<I> && Sentinel<S, I> &&
                IndirectRelation<equal_to, projected<I, P>, V const *>)
        I operator()(I begin, S end, V const &val, P proj = P{}) const
        {
            for(; begin != end; ++begin)
                if(invoke(proj, *begin) == val)
                    break;
            return begin;
        }

        /// \overload
        CPP_template(typename Rng, typename V, typename P = identity)(
            requires InputRange<Rng> &&
                IndirectRelation<equal_to, projected<iterator_t<Rng>, P>, V const *>)
        safe_iterator_t<Rng> operator()(Rng &&rng, V const &val, P proj = P{}) const
        {
            return (*this)(begin(rng), end(rng), val, std::move(proj));
        }
    };

    /// \sa `find_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(find_fn, find)

    namespace cpp20
    {
        using ranges::find;
    }
    /// @}
} // namespace ranges

#endif // include guard
