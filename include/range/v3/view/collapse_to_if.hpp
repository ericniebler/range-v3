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

#ifndef RANGES_V3_VIEW_COLLAPSE_TO_IF_HPP
#define RANGES_V3_VIEW_COLLAPSE_TO_IF_HPP

#include <functional>
#include <utility>
#include <concepts/concepts.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/functional/bind.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/functional/reference_wrapper.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/collapse_if.hpp>
#include <range/v3/view/replace_if.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace view
    {
        struct collapse_to_if_fn
        {
        private:
            friend view_access;
            template<typename Val, typename Pred>
            static auto bind(collapse_to_if_fn collapse_to_if, Val value, Pred pred)
            {
                return make_pipeable(
                    std::bind(collapse_to_if, std::placeholders::_1, protect(std::move(value)),
                    protect(std::move(pred))
                    ));
            }
            template<typename Val, typename Pred, typename Proj>
            static auto bind(collapse_to_if_fn collapse_to_if, Val value, Pred pred, Proj proj)
            {
                return make_pipeable(
                    std::bind(collapse_to_if, std::placeholders::_1, protect(std::move(value)),
                    protect(std::move(pred)), protect(std::move(proj))));
            }
        public:
            template<typename Rng, typename Val, typename Pred>
            auto operator()(Rng &&rng, Val value, Pred pred) const ->
                CPP_ret(replace_if_view<collapse_if_view<all_t<Rng>, Pred>, Pred, Val>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectUnaryPredicate<Pred, iterator_t<Rng>> &&
                        Common<detail::decay_t<unwrap_reference_t<Val const &>>, range_value_t<Rng>> &&
                        CommonReference<unwrap_reference_t<Val const &>, range_reference_t<Rng>> &&
                        CommonReference<unwrap_reference_t<Val const &>,
                            range_rvalue_reference_t<Rng>>)
            {
                return {{static_cast<Rng &&>(rng), pred}, {std::move(pred), std::move(value)}};
            }
            template<typename Rng, typename Val, typename Pred, typename Proj>
            auto operator()(Rng &&rng, Val value, Pred pred, Proj proj) const ->
                CPP_ret(replace_if_view<collapse_if_view<all_t<Rng>, composed<Pred, Proj>>,
                        composed<Pred, Proj>, Val>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectUnaryPredicate<composed<Pred, Proj>, iterator_t<Rng>> &&
                        Common<detail::decay_t<unwrap_reference_t<Val const &>>, range_value_t<Rng>> &&
                        CommonReference<unwrap_reference_t<Val const &>, range_reference_t<Rng>> &&
                        CommonReference<unwrap_reference_t<Val const &>,
                            range_rvalue_reference_t<Rng>>)
            {
                return {{static_cast<Rng &&>(rng), compose(pred, proj)},
                    {compose(std::move(pred), std::move(proj)), std::move(value)}};
            }
        };

        /// \relates collapse_to_if_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<collapse_to_if_fn>, collapse_to_if)
    }
    /// @}
}

#endif // RANGES_V3_VIEW_COLLAPSE_TO_IF_HPP
