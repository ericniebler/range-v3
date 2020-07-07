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
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_HPP

#include <meta/meta.hpp>

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

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using remove_copy_result = detail::in_out_result<I, O>;

    RANGES_FUNC_BEGIN(remove_copy)

        /// \brief function template \c remove_copy
        template<typename I, typename S, typename O, typename T, typename P = identity>
        auto RANGES_FUNC(remove_copy)(
            I first, S last, O out, T const & val, P proj = P{}) //
            ->CPP_ret(remove_copy_result<I, O>)(                 //
                requires input_iterator<I> && sentinel_for<S, I> &&
                weakly_incrementable<O> &&
                indirect_relation<equal_to, projected<I, P>, T const *> &&
                indirectly_copyable<I, O>)
        {
            for(; first != last; ++first)
            {
                auto && x = *first;
                if(!(invoke(proj, x) == val))
                {
                    *out = (decltype(x) &&)x;
                    ++out;
                }
            }
            return {first, out};
        }

        /// \overload
        template<typename Rng, typename O, typename T, typename P = identity>
        auto RANGES_FUNC(remove_copy)(Rng && rng, O out, T const & val, P proj = P{}) //
            ->CPP_ret(remove_copy_result<borrowed_iterator_t<Rng>, O>)(               //
                requires input_range<Rng> && weakly_incrementable<O> &&
                indirect_relation<equal_to, projected<iterator_t<Rng>, P>, T const *> &&
                indirectly_copyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out), val, std::move(proj));
        }

    RANGES_FUNC_END(remove_copy)

    namespace cpp20
    {
        using ranges::remove_copy;
        using ranges::remove_copy_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
