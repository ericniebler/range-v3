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

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

#include <iterator>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct indirect_view
      : view_adaptor<indirect_view<Rng>, Rng>
    {
    private:
        friend range_access;

        template<bool IsConst>
        struct adaptor
          : adaptor_base
        {
            friend adaptor<!IsConst>;
            using CRng = meta::const_if_c<IsConst, Rng>;

            adaptor() = default;
            CPP_template(bool Other)(
                requires IsConst && (!Other))
            constexpr adaptor(adaptor<Other>) noexcept
            {}

            constexpr auto CPP_auto_fun(read)(iterator_t<CRng> const &it) (const)
            (
                return **it
            )
            constexpr auto CPP_auto_fun(iter_move)(iterator_t<CRng> const &it) (const)
            (
                return ranges::iter_move(*it)
            )
        };

        CPP_member
        constexpr /*c++14*/ auto begin_adaptor() noexcept ->
            CPP_ret(adaptor<false>)(
                requires (!simple_view<Rng>()))
        {
            return {};
        }
        CPP_member
        constexpr auto begin_adaptor() const noexcept ->
            CPP_ret(adaptor<true>)(
                requires Range<Rng const>)
        {
            return {};
        }

        CPP_member
        constexpr /*c++14*/ auto end_adaptor() noexcept ->
            CPP_ret(adaptor<false>)(
                requires (!simple_view<Rng>()))
        {
            return {};
        }
        CPP_member
        constexpr auto end_adaptor() const noexcept ->
            CPP_ret(adaptor<true>)(
                requires Range<Rng const>)
        {
            return {};
        }
    public:
        indirect_view() = default;
        explicit constexpr indirect_view(Rng rng)
          : indirect_view::view_adaptor{detail::move(rng)}
        {}
        CPP_member
        constexpr auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return ranges::size(this->base());
        }
        CPP_member
        constexpr /*c++14*/ auto CPP_fun(size)() (
            requires SizedRange<Rng>)
        {
            return ranges::size(this->base());
        }
    };

    namespace view
    {
        struct indirect_fn
        {
            template<typename Rng>
            constexpr auto CPP_fun(operator())(Rng &&rng) (const
                requires ViewableRange<Rng> && InputRange<Rng> &&
                    // We shouldn't need to strip references to test if something
                    // is readable. https://github.com/ericniebler/stl2/issues/594
                    //Readable<range_reference_t<Rng>>)
                    ((bool) Readable<range_value_t<Rng>>)) // Cast to bool needed for GCC (???)
            {
                return indirect_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
        };

        /// \relates indirect_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<indirect_fn>, indirect)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::indirect_view)

#endif
