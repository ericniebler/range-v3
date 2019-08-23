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

#ifndef RANGES_V3_VIEW_REPLACE_IF_HPP
#define RANGES_V3_VIEW_REPLACE_IF_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/view.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Pred, typename Val>
        struct replacer_if_fn : compressed_pair<semiregular_box_t<Pred>, Val>
        {
        private:
            using base_t = compressed_pair<semiregular_box_t<Pred>, Val>;
            using base_t::first;
            using base_t::second;

        public:
            replacer_if_fn() = default;
            constexpr replacer_if_fn(Pred pred, Val new_value)
              : base_t{std::move(pred), std::move(new_value)}
            {}

            template<typename I>
            [[noreturn]] common_type_t<decay_t<unwrap_reference_t<Val const &>>,
                                       iter_value_t<I>> &
            operator()(copy_tag, I const &) const
            {
                RANGES_EXPECT(false);
            }

            template<typename I>
            auto operator()(I const & i)
                -> CPP_ret(common_reference_t<unwrap_reference_t<Val const &>,
                                              iter_reference_t<I>>)( //
                    requires(!invocable<Pred const &, iter_reference_t<I>>))
            {
                auto && x = *i;
                if(invoke(first(), (decltype(x) &&)x))
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }
            template<typename I>
            auto operator()(I const & i) const
                -> CPP_ret(common_reference_t<unwrap_reference_t<Val const &>,
                                              iter_reference_t<I>>)( //
                    requires invocable<Pred const &, iter_reference_t<I>>)
            {
                auto && x = *i;
                if(invoke(first(), (decltype(x) &&)x))
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }

            template<typename I>
            auto operator()(move_tag, I const & i)
                -> CPP_ret(common_reference_t<unwrap_reference_t<Val const &>,
                                              iter_rvalue_reference_t<I>>)( //
                    requires(!invocable<Pred const &, iter_rvalue_reference_t<I>>))
            {
                auto && x = iter_move(i);
                if(invoke(first(), (decltype(x) &&)x))
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }
            template<typename I>
            auto operator()(move_tag, I const & i) const
                -> CPP_ret(common_reference_t<unwrap_reference_t<Val const &>,
                                              iter_rvalue_reference_t<I>>)( //
                    requires invocable<Pred const &, iter_rvalue_reference_t<I>>)
            {
                auto && x = iter_move(i);
                if(invoke(first(), (decltype(x) &&)x))
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct replace_if_fn
        {
        private:
            friend view_access;
            template<typename Pred, typename Val>
            static constexpr auto bind(replace_if_fn replace_if, Pred pred, Val new_value)
            {
                return make_pipeable(
                    bind_back(replace_if, std::move(pred), std::move(new_value)));
            }

        public:
            template<typename Rng, typename Pred, typename Val>
            constexpr auto operator()(Rng && rng, Pred pred, Val new_value) const
                -> CPP_ret(replace_if_view<all_t<Rng>, Pred, Val>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        indirect_unary_predicate<Pred, iterator_t<Rng>> &&
                            common_with<detail::decay_t<unwrap_reference_t<Val const &>>,
                                        range_value_t<Rng>> &&
                                common_reference_with<unwrap_reference_t<Val const &>,
                                                      range_reference_t<Rng>> &&
                                    common_reference_with<unwrap_reference_t<Val const &>,
                                                          range_rvalue_reference_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)),
                        {std::move(pred), std::move(new_value)}};
            }
        };

        /// \relates replace_if_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<replace_if_fn>, replace_if)
    } // namespace views
    /// @}
} // namespace ranges

RANGES_RE_ENABLE_WARNINGS

#endif
