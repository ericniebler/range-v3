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

#ifndef RANGES_V3_VIEW_DROP_WHILE_HPP
#define RANGES_V3_VIEW_DROP_WHILE_HPP

#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct drop_while_view
      : view_interface<drop_while_view<Rng, Pred>,
                       is_finite<Rng>::value ? finite : unknown>
    {
    private:
        Rng rng_;
        RANGES_NO_UNIQUE_ADDRESS semiregular_box_t<Pred> pred_;
        detail::non_propagating_cache<iterator_t<Rng>> begin_;

        iterator_t<Rng> get_begin_()
        {
            if(!begin_)
                begin_ = find_if_not(rng_, std::ref(pred_));
            return *begin_;
        }

    public:
        drop_while_view() = default;
        drop_while_view(Rng rng, Pred pred)
          : rng_(std::move(rng))
          , pred_(std::move(pred))
        {}
        iterator_t<Rng> begin()
        {
            return get_begin_();
        }
        sentinel_t<Rng> end()
        {
            return ranges::end(rng_);
        }
        Rng base() const
        {
            return rng_;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Fun)(requires copy_constructible<Fun>)
        drop_while_view(Rng &&, Fun)
            ->drop_while_view<views::all_t<Rng>, Fun>;
#endif

    namespace views
    {
        struct drop_while_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static constexpr auto bind(drop_while_fn drop_while, Pred pred)
            {
                return make_pipeable(bind_back(drop_while, std::move(pred)));
            }
            template<typename Pred, typename Proj>
            static constexpr auto bind(drop_while_fn drop_while, Pred pred, Proj proj)
            {
                return make_pipeable(
                    bind_back(drop_while, std::move(pred), std::move(proj)));
            }

        public:
            template<typename Rng, typename Pred>
            auto operator()(Rng && rng, Pred pred) const
                -> CPP_ret(drop_while_view<all_t<Rng>, Pred>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<Pred, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
            template<typename Rng, typename Pred, typename Proj>
            auto operator()(Rng && rng, Pred pred, Proj proj) const
                -> CPP_ret(drop_while_view<all_t<Rng>, composed<Pred, Proj>>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<composed<Pred, Proj>, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)),
                        compose(std::move(pred), std::move(proj))};
            }
        };

        /// \relates drop_while_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<drop_while_fn>, drop_while)
    } // namespace views

    namespace cpp20
    {
        namespace views
        {
            using ranges::views::drop_while;
        }
        CPP_template(typename Rng, typename Pred)( //
            requires viewable_range<Rng> && input_range<Rng> &&
                indirect_unary_predicate<Pred, iterator_t<Rng>>) //
            using drop_while_view = ranges::drop_while_view<Rng, Pred>;
    } // namespace cpp20
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::drop_while_view)

#endif
