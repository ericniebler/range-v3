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

#ifndef RANGES_V3_VIEW_MOVE_HPP
#define RANGES_V3_VIEW_MOVE_HPP

#include <type_traits>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct move_view : view_adaptor<move_view<Rng>, Rng>
    {
    private:
        friend range_access;
        template<bool Const>
        struct adaptor : adaptor_base
        {
            adaptor() = default;
            CPP_template(bool Other)( //
                requires Const && (!Other)) constexpr adaptor(adaptor<Other>)
            {}
            using CRng = meta::const_if_c<Const, Rng>;
            using value_type = range_value_t<Rng>;
            range_rvalue_reference_t<CRng> read(iterator_t<CRng> const & it) const
            {
                return ranges::iter_move(it);
            }
            range_rvalue_reference_t<CRng> iter_move(iterator_t<CRng> const & it) const
            {
                return ranges::iter_move(it);
            }
        };
        adaptor<simple_view<Rng>()> begin_adaptor()
        {
            return {};
        }
        adaptor<simple_view<Rng>()> end_adaptor()
        {
            return {};
        }
        CPP_member
        auto begin_adaptor() const -> CPP_ret(adaptor<true>)( //
            requires input_range<Rng const>)
        {
            return {};
        }
        CPP_member
        auto end_adaptor() const -> CPP_ret(adaptor<true>)( //
            requires input_range<Rng const>)
        {
            return {};
        }

    public:
        move_view() = default;
        explicit move_view(Rng rng)
          : move_view::view_adaptor{std::move(rng)}
        {}
        CPP_member
        auto CPP_fun(size)()(const requires sized_range<Rng const>)
        {
            return ranges::size(this->base());
        }
        CPP_member
        auto CPP_fun(size)()(requires sized_range<Rng>)
        {
            return ranges::size(this->base());
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    move_view(Rng &&)->move_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct move_fn
        {
            template<typename Rng>
            auto operator()(Rng && rng) const -> CPP_ret(move_view<all_t<Rng>>)( //
                requires viewable_range<Rng> && input_range<Rng>)
            {
                return move_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
        };

        /// \relates move_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<move_fn>, move)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::move_view)

#endif
