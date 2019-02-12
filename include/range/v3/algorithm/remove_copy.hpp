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
    using remove_copy_result = detail::in_out_result<I, O>;

    struct remove_copy_fn
    {
        template<typename I, typename S, typename O, typename T, typename P = identity>
        auto operator()(I begin, S end, O out, T const &val, P proj = P{}) const ->
            CPP_ret(remove_copy_result<I, O>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    WeaklyIncrementable<O> &&
                    IndirectRelation<equal_to, projected<I, P>, T const *> &&
                    IndirectlyCopyable<I, O>)
        {
            for(; begin != end; ++begin)
            {
                auto &&x = *begin;
                if(!(invoke(proj, x) == val))
                {
                    *out = (decltype(x) &&) x;
                    ++out;
                }
            }
            return {begin, out};
        }

        template<typename Rng, typename O, typename T, typename P = identity>
        auto operator()(Rng &&rng, O out, T const &val, P proj = P{}) const ->
            CPP_ret(remove_copy_result<safe_iterator_t<Rng>, O>)(
                requires InputRange<Rng> && WeaklyIncrementable<O> &&
                    IndirectRelation<equal_to, projected<iterator_t<Rng>, P>, T const *> &&
                    IndirectlyCopyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out), val, std::move(proj));
        }
    };

    /// \sa `remove_copy_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(remove_copy_fn, remove_copy)

    namespace cpp20
    {
        using ranges::remove_copy_result;
        using ranges::remove_copy;
    }
    /// @}
} // namespace ranges

#endif // include guard
