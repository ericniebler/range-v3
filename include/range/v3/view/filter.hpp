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

#ifndef RANGES_V3_VIEW_FILTER_HPP
#define RANGES_V3_VIEW_FILTER_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/not_fn.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/remove_if.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct filter_view : remove_if_view<Rng, logical_negate<Pred>>
    {
        filter_view() = default;
        constexpr filter_view(Rng rng, Pred pred)
          : filter_view::remove_if_view{std::move(rng), not_fn(std::move(pred))}
        {}
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Pred)( //
        requires input_range<Rng> && indirect_unary_predicate<Pred, iterator_t<Rng>> &&
            view_<Rng> && std::is_object<Pred>::value) //
        filter_view(Rng &&, Pred)
            ->filter_view<views::all_t<Rng>, Pred>;
#endif

    namespace views
    {
        struct filter_fn;

        /// Given a source range and a unary predicate,
        /// present a view of the elements that satisfy the predicate.
        struct cpp20_filter_fn
        {
        private:
            friend view_access;
            friend filter_fn;
            template<typename Pred>
            static constexpr auto bind(cpp20_filter_fn filter, Pred pred)
            {
                return make_pipeable(bind_back(filter, std::move(pred)));
            }

        public:
            template<typename Rng, typename Pred>
            constexpr auto operator()(Rng && rng, Pred pred) const
                -> CPP_ret(filter_view<all_t<Rng>, Pred>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<Pred, iterator_t<Rng>>)
            {
                return filter_view<all_t<Rng>, Pred>{all(static_cast<Rng &&>(rng)),
                                                     std::move(pred)};
            }
        };

        /// Given a source range, unary predicate, and optional projection,
        /// present a view of the elements that satisfy the predicate.
        struct filter_fn : cpp20_filter_fn
        {
        private:
            friend view_access;
            using cpp20_filter_fn::bind;

            template<typename Pred, typename Proj>
            static constexpr auto bind(filter_fn filter, Pred pred, Proj proj)
            {
                return make_pipeable(bind_back(filter, std::move(pred), std::move(proj)));
            }

        public:
            using cpp20_filter_fn::operator();

            template<typename Rng, typename Pred, typename Proj>
            constexpr auto operator()(Rng && rng, Pred pred, Proj proj) const
                -> CPP_ret(filter_view<all_t<Rng>, composed<Pred, Proj>>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>>)
            {
                return filter_view<all_t<Rng>, composed<Pred, Proj>>{
                    all(static_cast<Rng &&>(rng)),
                    compose(std::move(pred), std::move(proj))};
            }
        };

        /// \relates filter_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<filter_fn>, filter)
    } // namespace views

    namespace cpp20
    {
        namespace views
        {
            RANGES_INLINE_VARIABLE(ranges::views::view<ranges::views::cpp20_filter_fn>,
                                   filter)
        }
        CPP_template(typename V, typename Pred)( //
            requires input_range<V> && indirect_unary_predicate<Pred, iterator_t<V>> &&
                view_<V> && std::is_object<Pred>::value) //
            using filter_view = ranges::filter_view<V, Pred>;
    } // namespace cpp20
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::filter_view)

#endif
