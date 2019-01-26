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

#ifndef RANGES_V3_VIEW_ADJACENT_FILTER_HPP
#define RANGES_V3_VIEW_ADJACENT_FILTER_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        CPP_def
        (
            template(typename Rng, typename Pred)
            concept AdjacentFilter,
                ViewableRange<Rng> && ForwardRange<Rng> &&
                IndirectBinaryPredicate<Pred, iterator_t<Rng>, iterator_t<Rng>>
        );
    }
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct RANGES_EMPTY_BASES adjacent_filter_view
      : view_adaptor<
            adjacent_filter_view<Rng, Pred>,
            Rng,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
      , private box<semiregular_t<Pred>, adjacent_filter_view<Rng, Pred>>
    {
    private:
        friend range_access;

        template<bool Const>
        struct adaptor
          : adaptor_base
        {
        private:
            friend struct adaptor<!Const>;
            using CRng = meta::const_if_c<Const, Rng>;
            using Parent = meta::const_if_c<Const, adjacent_filter_view>;
            Parent *rng_;
        public:
            adaptor() = default;
            constexpr adaptor(Parent &rng) noexcept
              : rng_(&rng)
            {}
            template<bool Other>
            constexpr CPP_ctor(adaptor)(adaptor<Other> that)(
                requires Const && (!Other))
              : rng_(that.rng_)
            {}
            constexpr /*c++14*/ void next(iterator_t<CRng> &it) const
            {
                auto const last = ranges::end(rng_->base());
                auto &pred = rng_->adjacent_filter_view::box::get();
                RANGES_EXPECT(it != last);
                for(auto prev = it; ++it != last; prev = it)
                    if(invoke(pred, *prev, *it))
                        break;
            }
            CPP_member
            constexpr /*c++14*/ auto prev(iterator_t<CRng> &it) const ->
                CPP_ret(void)(
                    requires BidirectionalRange<CRng>)
            {
                auto const first = ranges::begin(rng_->base());
                auto &pred = rng_->adjacent_filter_view::box::get();
                RANGES_EXPECT(it != first);
                --it;
                while(it != first)
                {
                    auto prev = it;
                    if(invoke(pred, *--prev, *it))
                        break;
                    it = prev;
                }
            }
            void distance_to() = delete;
        };
        constexpr auto begin_adaptor() noexcept ->
            adaptor<false>
        {
            return {*this};
        }
        CPP_member
        constexpr auto begin_adaptor() const noexcept ->
            CPP_ret(adaptor<true>)(
                requires detail::AdjacentFilter<Rng const, Pred const>)
        {
            return {*this};
        }
        constexpr auto end_adaptor() noexcept ->
            adaptor<false>
        {
            return {*this};
        }
        CPP_member
        constexpr auto end_adaptor() const noexcept ->
            CPP_ret(adaptor<true>)(
                requires detail::AdjacentFilter<Rng const, Pred const>)
        {
            return {*this};
        }
    public:
        adjacent_filter_view() = default;
        constexpr adjacent_filter_view(Rng rng, Pred pred)
          : adjacent_filter_view::view_adaptor{detail::move(rng)}
          , adjacent_filter_view::box(detail::move(pred))
        {}
    };

    namespace view
    {
        struct adjacent_filter_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            constexpr /*c++14*/
            static auto bind(adjacent_filter_fn adjacent_filter, Pred pred)
            {
                return make_pipeable(std::bind(adjacent_filter, std::placeholders::_1,
                    protect(std::move(pred))));
            }
        public:
            template<typename Rng, typename Pred>
            constexpr /*c++14*/ auto operator()(Rng &&rng, Pred pred) const ->
                CPP_ret(adjacent_filter_view<all_t<Rng>, Pred>)(
                    requires detail::AdjacentFilter<Rng, Pred>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
        };

        /// \relates adjacent_filter_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<adjacent_filter_fn>, adjacent_filter)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::adjacent_filter_view)

#endif
