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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using replace_copy_if_result = detail::in_out_result<I, O>;

    struct replace_copy_if_fn
    {
        template<typename I, typename S, typename O, typename C, typename T, typename P = identity>
        auto operator()(I begin, S end, O out, C pred, T const & new_value, P proj = {}) const ->
            CPP_ret(replace_copy_if_result<I, O>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    OutputIterator<O, T const &> &&
                    IndirectUnaryPredicate<C, projected<I, P>> &&
                    IndirectlyCopyable<I, O>)
        {
            for(; begin != end; ++begin, ++out)
            {
                auto &&x = *begin;
                if(invoke(pred, invoke(proj, x)))
                    *out = new_value;
                else
                    *out = (decltype(x) &&) x;
            }
            return {begin, out};
        }

        template<typename Rng, typename O, typename C, typename T, typename P = identity>
        auto operator()(Rng &&rng, O out, C pred, T const & new_value, P proj = {}) const ->
            CPP_ret(replace_copy_if_result<safe_iterator_t<Rng>, O>)(
                requires InputRange<Rng> && OutputIterator<O, T const &> &&
                    IndirectUnaryPredicate<C, projected<iterator_t<Rng>, P>> &&
                    IndirectlyCopyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out), std::move(pred), new_value,
                std::move(proj));
        }
    };

    /// \sa `replace_copy_if_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(replace_copy_if_fn, replace_copy_if)

    namespace cpp20
    {
        using ranges::replace_copy_if_result;
        using ranges::replace_copy_if;
    }
    /// @}
} // namespace ranges

#endif // include guard
