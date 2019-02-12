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
#ifndef RANGES_V3_ALGORITHM_ROTATE_COPY_HPP
#define RANGES_V3_ALGORITHM_ROTATE_COPY_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using rotate_copy_result = detail::in_out_result<I, O>;

    struct rotate_copy_fn
    {
        template<typename I, typename S, typename O, typename P = identity>
        auto operator()(I begin, I middle, S end, O out) const ->
            CPP_ret(rotate_copy_result<I, O>)(
                requires ForwardIterator<I> && Sentinel<S, I> && WeaklyIncrementable<O> && IndirectlyCopyable<I, O>)
        {
            auto res = ranges::copy(middle, std::move(end), std::move(out));
            return {
                std::move(res.in),
                ranges::copy(std::move(begin), middle, std::move(res.out)).out
            };
        }

        template<typename Rng, typename O, typename P = identity>
        auto operator()(Rng &&rng, iterator_t<Rng> middle, O out) const ->
            CPP_ret(rotate_copy_result<safe_iterator_t<Rng>, O>)(
                requires Range<Rng> && WeaklyIncrementable<O> &&
                    IndirectlyCopyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), std::move(middle), end(rng), std::move(out));
        }
    };

    /// \sa `rotate_copy_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(rotate_copy_fn, rotate_copy)

    namespace cpp20
    {
        using ranges::rotate_copy_result;
        using ranges::rotate_copy;
    }
    /// @}
} // namespace ranges

#endif // include guard
