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
#ifndef RANGES_V3_ALGORITHM_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_COPY_IF_HPP

#include <functional>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/result_types.hpp>
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
    template<typename I, typename O>
    using copy_if_result = detail::in_out_result<I, O>;

    RANGES_BEGIN_NIEBLOID(copy_if)

        /// \brief function template \c copy_if
        template<typename I, typename S, typename O, typename F, typename P = identity>
        auto RANGES_FUN_NIEBLOID(copy_if)(I first, S last, O out, F pred, P proj = P{}) //
            ->CPP_ret(copy_if_result<I, O>)(                                            //
                requires input_iterator<I> && sentinel_for<S, I> &&
                weakly_incrementable<O> && indirect_unary_predicate<F, projected<I, P>> &&
                indirectly_copyable<I, O>)
        {
            for(; first != last; ++first)
            {
                auto && x = *first;
                if(invoke(pred, invoke(proj, x)))
                {
                    *out = (decltype(x) &&)x;
                    ++out;
                }
            }
            return {first, out};
        }

        /// \overload
        template<typename Rng, typename O, typename F, typename P = identity>
        auto RANGES_FUN_NIEBLOID(copy_if)(Rng && rng, O out, F pred, P proj = P{})
            ->CPP_ret(copy_if_result<safe_iterator_t<Rng>, O>)( //
                requires input_range<Rng> && weakly_incrementable<O> &&
                indirect_unary_predicate<F, projected<iterator_t<Rng>, P>> &&
                indirectly_copyable<iterator_t<Rng>, O>)
        {
            return (*this)(
                begin(rng), end(rng), std::move(out), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(copy_if)

    namespace cpp20
    {
        using ranges::copy_if;
        using ranges::copy_if_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
