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

#ifndef RANGES_V3_VIEW_TAIL_HPP
#define RANGES_V3_VIEW_TAIL_HPP

#include <type_traits>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    namespace detail
    {
        template<typename T>
        constexpr T prev_or_zero_(T n)
        {
            return n == 0 ? T(0) : T(n - 1);
        }
    } // namespace detail

    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct tail_view
      : view_interface<tail_view<Rng>,
                       (range_cardinality<Rng>::value >= 0)
                           ? detail::prev_or_zero_(range_cardinality<Rng>::value)
                           : range_cardinality<Rng>::value>
    {
    private:
        Rng rng_;

    public:
        tail_view() = default;
        tail_view(Rng rng)
          : rng_(static_cast<Rng &&>(rng))
        {
            CPP_assert(input_range<Rng>);
        }
        iterator_t<Rng> begin()
        {
            return next(ranges::begin(rng_), 1, ranges::end(rng_));
        }
        template<bool Const = true>
        auto begin() const -> CPP_ret(iterator_t<meta::const_if_c<Const, Rng>>)( //
            requires Const && range<meta::const_if_c<Const, Rng>>)
        {
            return next(ranges::begin(rng_), 1, ranges::end(rng_));
        }
        sentinel_t<Rng> end()
        {
            return ranges::end(rng_);
        }
        template<bool Const = true>
        auto end() const -> CPP_ret(sentinel_t<meta::const_if_c<Const, Rng>>)( //
            requires Const && range<meta::const_if_c<Const, Rng>>)
        {
            return ranges::end(rng_);
        }
        CPP_member
        constexpr auto CPP_fun(size)()(requires sized_range<Rng>)
        {
            using size_type = range_size_t<Rng>;
            return range_cardinality<Rng>::value >= 0
                       ? detail::prev_or_zero_((size_type)range_cardinality<Rng>::value)
                       : detail::prev_or_zero_(ranges::size(rng_));
        }
        CPP_member
        constexpr auto CPP_fun(size)()(const requires sized_range<Rng const>)
        {
            using size_type = range_size_t<Rng>;
            return range_cardinality<Rng>::value >= 0
                       ? detail::prev_or_zero_((size_type)range_cardinality<Rng>::value)
                       : detail::prev_or_zero_(ranges::size(rng_));
        }
        Rng base() const
        {
            return rng_;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng)(requires viewable_range<Rng>) tail_view(Rng &&)
        ->tail_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct tail_fn
        {
            template<typename Rng>
            auto operator()(Rng && rng) const
                -> CPP_ret(meta::if_c<range_cardinality<Rng>::value == 0, all_t<Rng>,
                                      tail_view<all_t<Rng>>>)( //
                    requires viewable_range<Rng> && input_range<Rng>)
            {
                return all(static_cast<Rng &&>(rng));
            }
        };

        /// \relates tail_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<tail_fn>, tail)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::tail_view)

#endif
