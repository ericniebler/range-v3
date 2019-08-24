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

#ifndef RANGES_V3_VIEW_TAKE_WHILE_HPP
#define RANGES_V3_VIEW_TAKE_WHILE_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/indirect.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct iter_take_while_view
      : view_adaptor<iter_take_while_view<Rng, Pred>, Rng,
                     is_finite<Rng>::value ? finite : unknown>
    {
    private:
        friend range_access;
        RANGES_NO_UNIQUE_ADDRESS semiregular_box_t<Pred> pred_;

        template<bool IsConst>
        struct sentinel_adaptor : adaptor_base
        {
        private:
            friend struct sentinel_adaptor<!IsConst>;
            using CRng = meta::const_if_c<IsConst, Rng>;
            semiregular_box_ref_or_val_t<Pred, IsConst> pred_;

        public:
            sentinel_adaptor() = default;
            sentinel_adaptor(semiregular_box_ref_or_val_t<Pred, IsConst> pred)
              : pred_(std::move(pred))
            {}
            CPP_template(bool Other)( //
                requires IsConst && (!Other))
                sentinel_adaptor(sentinel_adaptor<Other> that)
              : pred_(std::move(that.pred_))
            {}
            bool empty(iterator_t<CRng> const & it, sentinel_t<CRng> const & last) const
            {
                return it == last || !invoke(pred_, it);
            }
        };
        sentinel_adaptor<false> end_adaptor()
        {
            return {pred_};
        }
        template<bool Const = true>
        auto end_adaptor() const -> CPP_ret(sentinel_adaptor<Const>)( //
            requires Const && range<meta::const_if_c<Const, Rng>> &&
                invocable<Pred const &, iterator_t<meta::const_if_c<Const, Rng>>>)
        {
            return {pred_};
        }

    public:
        iter_take_while_view() = default;
        constexpr iter_take_while_view(Rng rng, Pred pred)
          : iter_take_while_view::view_adaptor{std::move(rng)}
          , pred_(std::move(pred))
        {}
    };

    template<typename Rng, typename Pred>
    struct take_while_view : iter_take_while_view<Rng, indirected<Pred>>
    {
        take_while_view() = default;
        constexpr take_while_view(Rng rng, Pred pred)
          : iter_take_while_view<Rng, indirected<Pred>>{std::move(rng),
                                                        indirect(std::move(pred))}
        {}
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Fun)(requires copy_constructible<Fun>)
        take_while_view(Rng &&, Fun)
            ->take_while_view<views::all_t<Rng>, Fun>;
#endif

    namespace views
    {
        struct iter_take_while_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static constexpr auto bind(iter_take_while_fn iter_take_while, Pred pred)
            {
                return make_pipeable(bind_back(iter_take_while, std::move(pred)));
            }

        public:
            template<typename Rng, typename Pred>
            constexpr auto operator()(Rng && rng, Pred pred) const
                -> CPP_ret(iter_take_while_view<all_t<Rng>, Pred>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        predicate<Pred &, iterator_t<Rng>> && copy_constructible<Pred>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
        };

        struct take_while_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static constexpr auto bind(take_while_fn take_while, Pred pred)
            {
                return make_pipeable(bind_back(take_while, std::move(pred)));
            }
            template<typename Pred, typename Proj>
            static constexpr auto bind(take_while_fn take_while, Pred pred, Proj proj)
            {
                return make_pipeable(
                    bind_back(take_while, std::move(pred), std::move(proj)));
            }

        public:
            template<typename Rng, typename Pred>
            constexpr auto operator()(Rng && rng, Pred pred) const
                -> CPP_ret(take_while_view<all_t<Rng>, Pred>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<Pred &, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
            template<typename Rng, typename Pred, typename Proj>
            constexpr auto operator()(Rng && rng, Pred pred, Proj proj) const
                -> CPP_ret(take_while_view<all_t<Rng>, composed<Pred, Proj>>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<composed<Pred, Proj> &, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)),
                        compose(std::move(pred), std::move(proj))};
            }
        };

        /// \relates iter_take_while_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<iter_take_while_fn>, iter_take_while)

        /// \relates take_while_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<take_while_fn>, take_while)
    } // namespace views

    namespace cpp20
    {
        namespace views
        {
            using ranges::views::take_while;
        }
        CPP_template(typename Rng, typename Pred)( //
            requires viewable_range<Rng> && input_range<Rng> &&
                predicate<Pred &, iterator_t<Rng>> && copy_constructible<Pred>) //
            using take_while_view = ranges::take_while_view<Rng, Pred>;
    } // namespace cpp20
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::iter_take_while_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::take_while_view)

#endif
