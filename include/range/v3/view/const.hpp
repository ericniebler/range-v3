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

#ifndef RANGES_V3_VIEW_CONST_HPP
#define RANGES_V3_VIEW_CONST_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct const_view
      : view_adaptor<const_view<Rng>, Rng>
    {
    private:
        friend range_access;
        template<bool Const>
        struct adaptor
          : adaptor_base
        {
            using CRng = meta::const_if_c<Const, Rng>;
            using value_ =
                range_value_t<CRng>;
            using reference_ =
                common_reference_t<value_ const &&, range_reference_t<CRng>>;
            using rvalue_reference_ =
                common_reference_t<value_ const &&, range_rvalue_reference_t<CRng>>;
            adaptor() = default;
            template<bool Other>
            constexpr CPP_ctor(adaptor)(adaptor<Other>)(
                requires Const && (!Other))
            {}
            reference_ read(iterator_t<CRng> const &it) const
            {
                return *it;
            }
            rvalue_reference_ iter_move(iterator_t<CRng> const &it) const
                noexcept(noexcept(rvalue_reference_(ranges::iter_move(it))))
            {
                return ranges::iter_move(it);
            }
        };
        adaptor<simple_view<Rng>()> begin_adaptor()
        {
            return {};
        }
        CPP_member
        auto begin_adaptor() const -> CPP_ret(adaptor<true>)(
            requires Range<Rng const>)
        {
            return {};
        }
        adaptor<simple_view<Rng>()> end_adaptor()
        {
            return {};
        }
        CPP_member
        auto end_adaptor() const -> CPP_ret(adaptor<true>)(
            requires Range<Rng const>)
        {
            return {};
        }
    public:
        const_view() = default;
        explicit const_view(Rng rng)
          : const_view::view_adaptor{std::move(rng)}
        {}
        CPP_member
        constexpr /*c++14*/ auto CPP_fun(size)() (
            requires SizedRange<Rng>)
        {
            return ranges::size(this->base());
        }
        CPP_member
        constexpr auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return ranges::size(this->base());
        }
    };

    namespace view
    {
        struct const_fn
        {
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(const_view<all_t<Rng>>)(
                    requires ViewableRange<Rng> && InputRange<Rng>)
            {
                return const_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
        };

        /// \relates const_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<const_fn>, const_)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::const_view)

#endif
