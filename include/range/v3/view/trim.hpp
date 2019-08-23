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

#ifndef RANGES_V3_VIEW_TRIM_HPP
#define RANGES_V3_VIEW_TRIM_HPP

#include <utility>

#include <concepts/concepts.hpp>

#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct trim_view : view_interface<trim_view<Rng, Pred>>
    {
    private:
        Rng rng_;
        semiregular_box_t<Pred> pred_;
        detail::non_propagating_cache<iterator_t<Rng>> begin_;
        detail::non_propagating_cache<iterator_t<Rng>> end_;

    public:
        CPP_assert(bidirectional_range<Rng> && view_<Rng> && indirect_unary_predicate<
                   Pred, iterator_t<Rng>> && common_range<Rng>);

        trim_view() = default;
        constexpr trim_view(Rng rng, Pred pred)
          : rng_(std::move(rng))
          , pred_(std::move(pred))
        {}

        iterator_t<Rng> begin()
        {
            if(!begin_)
                begin_ = find_if_not(rng_, std::ref(pred_));
            return *begin_;
        }
        iterator_t<Rng> end()
        {
            if(!end_)
            {
                const auto first = begin();
                auto last = ranges::end(rng_);
                while(last != first)
                    if(!invoke(pred_, *--last))
                    {
                        ++last;
                        break;
                    }
                end_ = std::move(last);
            }
            return *end_;
        }

        Rng base() const
        {
            return rng_;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng, typename Pred>
    trim_view(Rng &&, Pred)->trim_view<views::all_t<Rng>, Pred>;
#endif

    template<typename Rng, typename Pred>
    RANGES_INLINE_VAR constexpr bool disable_sized_range<trim_view<Rng, Pred>> = true;

    namespace views
    {
        struct trim_fn
        {
        private:
            friend view_access;
            template<typename Pred>
            static constexpr auto bind(trim_fn trim, Pred pred)
            {
                return make_pipeable(bind_back(trim, std::move(pred)));
            }
            template<typename Pred, typename Proj>
            static constexpr auto bind(trim_fn trim, Pred pred, Proj proj)
            {
                return make_pipeable(bind_back(trim, std::move(pred), std::move(proj)));
            }

        public:
            template<typename Rng, typename Pred>
            constexpr auto operator()(Rng && rng, Pred pred) const -> CPP_ret(
                trim_view<all_t<Rng>, Pred>)( //
                requires viewable_range<Rng> && bidirectional_range<Rng> &&
                    indirect_unary_predicate<Pred, iterator_t<Rng>> && common_range<Rng>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(pred)};
            }
            template<typename Rng, typename Pred, typename Proj>
            constexpr auto operator()(Rng && rng, Pred pred, Proj proj) const
                -> CPP_ret(trim_view<all_t<Rng>, composed<Pred, Proj>>)( //
                    requires viewable_range<Rng> && bidirectional_range<Rng> &&
                        indirect_unary_predicate<composed<Pred, Proj>, iterator_t<Rng>> &&
                            common_range<Rng>)
            {
                return {all(static_cast<Rng &&>(rng)),
                        compose(std::move(pred), std::move(proj))};
            }
        };

        /// \relates trim_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<trim_fn>, trim)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::trim_view)

#endif // RANGES_V3_VIEW_TRIM_HPP
