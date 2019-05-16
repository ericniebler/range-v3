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

#include <utility>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/indirect.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct iter_take_while_view
      : view_adaptor<
            iter_take_while_view<Rng, Pred>,
            Rng,
            is_finite<Rng>::value ? finite : unknown>
    {
    private:
        friend range_access;
        semiregular_t<Pred> pred_;

        template<bool IsConst>
        struct sentinel_adaptor
          : adaptor_base
        {
        private:
            friend struct sentinel_adaptor<!IsConst>;
            using CRng = meta::const_if_c<IsConst, Rng>;
            semiregular_ref_or_val_t<Pred, IsConst> pred_;
        public:
            sentinel_adaptor() = default;
            sentinel_adaptor(semiregular_ref_or_val_t<Pred, IsConst> pred)
              : pred_(std::move(pred))
            {}
            CPP_template(bool Other)(
                requires IsConst && (!Other))
            sentinel_adaptor(sentinel_adaptor<Other> that)
              : pred_(std::move(that.pred_))
            {}
            bool empty(iterator_t<CRng> const &it, sentinel_t<CRng> const &end) const
            {
                return it == end || !invoke(pred_, it);
            }
        };
        sentinel_adaptor<false> end_adaptor()
        {
            return {pred_};
        }
        template<bool Const = true>
        auto end_adaptor() const ->
            CPP_ret(sentinel_adaptor<Const>)(
                requires Const && Range<meta::const_if_c<Const, Rng>> &&
                    Invocable<Pred const &, iterator_t<meta::const_if_c<Const, Rng>>>)
        {
            return {pred_};
        }
    public:
        iter_take_while_view() = default;
        iter_take_while_view(Rng rng, Pred pred)
          : iter_take_while_view::view_adaptor{std::move(rng)}
          , pred_(std::move(pred))
        {}
    };

    template<typename Rng, typename Pred>
    struct take_while_view
      : iter_take_while_view<Rng, indirected<Pred>>
    {
        take_while_view() = default;
        take_while_view(Rng rng, Pred pred)
          : iter_take_while_view<Rng, indirected<Pred>>{std::move(rng),
                indirect(std::move(pred))}
        {}
    };

    namespace view
    {
        struct iter_take_while_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static auto bind(iter_take_while_fn iter_take_while, Pred pred)
            {
                return make_pipeable(std::bind(iter_take_while, std::placeholders::_1,
                    protect(std::move(pred))));
            }
        public:
            template<typename Rng, typename Pred>
            auto operator()(Rng &&rng, Pred pred) const ->
                CPP_ret(iter_take_while_view<all_t<Rng>, Pred>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        Predicate<Pred&, iterator_t<Rng>> &&
                        CopyConstructible<Pred>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
        };

        struct take_while_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static auto bind(take_while_fn take_while, Pred pred)
            {
                return make_pipeable(std::bind(take_while, std::placeholders::_1,
                    protect(std::move(pred))));
            }
            template<typename Pred, typename Proj>
            static auto bind(take_while_fn take_while, Pred pred, Proj proj)
            {
                return make_pipeable(std::bind(take_while, std::placeholders::_1,
                    protect(std::move(pred)), protect(std::move(proj))));
            }
        public:
            template<typename Rng, typename Pred>
            auto operator()(Rng &&rng, Pred pred) const ->
                CPP_ret(take_while_view<all_t<Rng>, Pred>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectUnaryPredicate<Pred &, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
            template<typename Rng, typename Pred, typename Proj>
            auto operator()(Rng &&rng, Pred pred, Proj proj) const ->
                CPP_ret(take_while_view<all_t<Rng>, composed<Pred, Proj>>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectUnaryPredicate<composed<Pred, Proj> &, iterator_t<Rng>>)
            {
                return {
                    all(static_cast<Rng &&>(rng)),
                    compose(std::move(pred), std::move(proj))
                };
            }
        };

        /// \relates iter_take_while_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<iter_take_while_fn>, iter_take_while)

        /// \relates take_while_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<take_while_fn>, take_while)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::iter_take_while_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::take_while_view)

#endif
