/// \file
// Range v3 library
//
//  Copyright Johel Guerrero 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_COLLAPSE_IF_HPP
#define RANGES_V3_VIEW_COLLAPSE_IF_HPP

#include <functional>
#include <type_traits>
#include <utility>
#include <concepts/concepts.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/functional/bind.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct collapse_if_view
      : view_adaptor<collapse_if_view<Rng, Pred>, Rng,
            range_cardinality<Rng>::value <= cardinality(1)
                ? range_cardinality<Rng>::value
                : finite>
    {
        CPP_assert(InputView<Rng> &&
            IndirectUnaryPredicate<Pred, iterator_t<Rng>>);

        collapse_if_view() = default;
        constexpr collapse_if_view(Rng rng, Pred pred)
          : collapse_if_view::view_adaptor(std::move(rng))
          , pred_(std::move(pred))
        {}
    private:
        friend range_access;

        struct adaptor : adaptor_base
        {
            adaptor() = default;
            constexpr adaptor(collapse_if_view& rng) noexcept
              : rng_{&rng}
            {}
            constexpr /*c++14*/ void next(iterator_t<Rng>& it) const
            {
                auto const last = ranges::end(rng_->base());
                RANGES_ASSERT(it != last);

                if (invoke(rng_->pred_, *it))
                {
                    do
                        ++it;
                    while (it != last && invoke(rng_->pred_, *it));
                }
                else
                    ++it;
            }
            CPP_member
            constexpr /*c++14*/ auto prev(iterator_t<Rng>& it) const ->
                CPP_ret(void)(
                    requires BidirectionalRange<Rng>)
            {
                auto const first = ranges::begin(rng_->base());
                RANGES_ASSERT(it != first); (void)first;

                if (invoke(rng_->pred_, *--it))
                {
                    while(it != first)
                        if (!invoke(rng_->pred_, *--it))
                        {
                            ++it;
                            break;
                        }
                }
            }
            void advance()     = delete;
            void distance_to() = delete;
        private:
            collapse_if_view* rng_;
        };
        constexpr adaptor begin_adaptor()
        {
            return {*this};
        }
        CPP_member
        constexpr auto end_adaptor() const noexcept ->
            CPP_ret(adaptor_base)(
                requires (not CommonRange<Rng>))
        {
            return {};
        }
        CPP_member
        constexpr auto end_adaptor() ->
            CPP_ret(adaptor)(
                requires CommonRange<Rng>)
        {
            return {*this};
        }

        semiregular_t<Pred> pred_;
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng, typename Pred>
    collapse_if_view(Rng &&, Pred) -> collapse_if_view<view::all_t<Rng>, Pred>;
#endif

    namespace view
    {
        struct collapse_if_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static auto bind(collapse_if_fn collapse_if, Pred pred)
            {
                return make_pipeable(
                    std::bind(collapse_if, std::placeholders::_1, protect(std::move(pred))));
            }
            template<typename Pred, typename Proj>
            static auto bind(collapse_if_fn collapse_if, Pred pred, Proj proj)
            {
                return make_pipeable(
                    std::bind(collapse_if, std::placeholders::_1, protect(std::move(pred)),
                    protect(std::move(proj))));
            }
        public:
            template<typename Rng, typename Pred>
            auto operator()(Rng &&rng, Pred pred) const ->
                CPP_ret(collapse_if_view<all_t<Rng>, Pred>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectUnaryPredicate<Pred, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
            template<typename Rng, typename Pred, typename Proj>
            auto operator()(Rng &&rng, Pred pred, Proj proj) const ->
                CPP_ret(collapse_if_view<all_t<Rng>, composed<Pred, Proj>>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectUnaryPredicate<composed<Pred, Proj>, iterator_t<Rng>>)
            {
                return {
                    all(static_cast<Rng &&>(rng)),
                    compose(std::move(pred), std::move(proj))
                };
            }
        };

        /// \relates collapse_if_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<collapse_if_fn>, collapse_if)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::collapse_if_view)

#endif // RANGES_V3_VIEW_COLLAPSE_IF_HPP
