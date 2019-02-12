/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_COPY_HPP
#define RANGES_V3_ALGORITHM_COPY_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using copy_result = detail::in_out_result<I, O>;

    struct cpp20_copy_fn
    {
        template<typename I, typename S, typename O>
        constexpr /*c++14*/ auto operator()(I begin, S end, O out) const ->
            CPP_ret(copy_result<I, O>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    WeaklyIncrementable<O> && IndirectlyCopyable<I, O>)
        {
            for(; begin != end; ++begin, ++out)
                *out = *begin;
            return {begin, out};
        }

        template<typename Rng, typename O>
        constexpr /*c++14*/ auto operator()(Rng &&rng, O out) const ->
            CPP_ret(copy_result<safe_iterator_t<Rng>, O>)(
                requires InputRange<Rng> && WeaklyIncrementable<O> &&
                    IndirectlyCopyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }
    };

    struct RANGES_EMPTY_BASES copy_fn
      : aux::copy_fn
      , cpp20_copy_fn
    {
        using aux::copy_fn::operator();
        using cpp20_copy_fn::operator();
    };

    /// \sa `copy_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(copy_fn, copy)

    namespace cpp20
    {
        using ranges::copy_result;
        RANGES_INLINE_VARIABLE(cpp20_copy_fn, copy)
    }
    /// @}
} // namespace ranges

#endif // include guard
