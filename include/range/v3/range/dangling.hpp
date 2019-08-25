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

#ifndef RANGES_V3_RANGE_DANGLING_HPP
#define RANGES_V3_RANGE_DANGLING_HPP

#include <utility>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \ingroup group-range
    /// A placeholder for an iterator or a sentinel into a range that may
    /// no longer be valid.
    struct dangling
    {
        dangling() = default;
        /// Implicit converting constructor; ignores argument
        template<typename T>
        constexpr CPP_ctor(dangling)(T &&)( //
            requires not_same_as_<T, dangling>)
        {}
    };

    /// \cond
    namespace detail
    {
        CPP_template(class R, class U)( //
            requires range<R>)          //
            using maybe_dangling_ = if_then_t<forwarding_range_<R>, U, dangling>;
    }
    /// \endcond

    template<typename Rng>
    using safe_iterator_t = detail::maybe_dangling_<Rng, iterator_t<Rng>>;

    /// \cond
    struct _sanitize_fn
    {
        template<typename T>
        constexpr T && operator()(T && t) const noexcept
        {
            return static_cast<T &&>(t);
        }
    };

    using sanitize_fn RANGES_DEPRECATED(
        "The sanitize function is unneeded and deprecated.") = _sanitize_fn;

    namespace
    {
        RANGES_DEPRECATED("The sanitize function is unneeded and deprecated.")
        constexpr auto & sanitize = static_const<_sanitize_fn>::value;
    } // namespace
    /// \endcond

    namespace cpp20
    {
        using ranges::dangling;
        using ranges::safe_iterator_t;
    } // namespace cpp20
} // namespace ranges

#endif
