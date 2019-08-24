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

#ifndef RANGES_V3_VIEW_GROUP_BY_HPP
#define RANGES_V3_VIEW_GROUP_BY_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    // TODO group_by could support Input ranges by keeping mutable state in
    // the range itself. The group_by view would then be mutable-only and
    // Input.

    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Fun>
    struct group_by_view
      : view_facade<group_by_view<Rng, Fun>,
                    is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        friend range_access;
        Rng rng_;
        semiregular_box_t<Fun> fun_;

        template<bool IsConst>
        struct cursor
        {
        private:
            friend struct cursor<!IsConst>;
            friend range_access;
            friend group_by_view;
            using CRng = meta::const_if_c<IsConst, Rng>;
            iterator_t<CRng> cur_;
            sentinel_t<CRng> last_;
            semiregular_box_ref_or_val_t<Fun, IsConst> fun_;

            struct pred
            {
                iterator_t<CRng> first_;
                semiregular_box_ref_or_val_t<Fun, IsConst> fun_;
                bool operator()(range_reference_t<CRng> r) const
                {
                    return invoke(fun_, *first_, r);
                }
            };
#ifdef RANGES_WORKAROUND_MSVC_787074
            template<bool Const = IsConst>
            auto read() const
                -> take_while_view<subrange<iterator_t<meta::const_if_c<Const, Rng>>,
                                            sentinel_t<meta::const_if_c<Const, Rng>>>,
                                   pred>
#else  // ^^^ workaround / no workaround vvv
            auto read() const
                -> take_while_view<subrange<iterator_t<CRng>, sentinel_t<CRng>>, pred>
#endif // RANGES_WORKAROUND_MSVC_787074
            {
                return {{cur_, last_}, {cur_, fun_}};
            }
            void next()
            {
                cur_ = find_if_not(cur_, last_, pred{cur_, fun_});
            }
            bool equal(default_sentinel_t) const
            {
                return cur_ == last_;
            }
            bool equal(cursor const & that) const
            {
                return cur_ == that.cur_;
            }
            cursor(semiregular_box_ref_or_val_t<Fun, IsConst> fun, iterator_t<CRng> first,
                   sentinel_t<CRng> last)
              : cur_(first)
              , last_(last)
              , fun_(fun)
            {}

        public:
            cursor() = default;
            CPP_template(bool Other)( //
                requires IsConst && (!Other)) cursor(cursor<Other> that)
              : cur_(std::move(that.cur_))
              , last_(std::move(last_))
              , fun_(std::move(that.fun_))
            {}
        };
        cursor<false> begin_cursor()
        {
            return {fun_, ranges::begin(rng_), ranges::end(rng_)};
        }
        template<bool Const = true>
        auto begin_cursor() const -> CPP_ret(cursor<Const>)( //
            requires Const && range<meta::const_if_c<Const, Rng>> && invocable<
                Fun const &, range_common_reference_t<meta::const_if_c<Const, Rng>>,
                range_common_reference_t<meta::const_if_c<Const, Rng>>>)
        {
            return {fun_, ranges::begin(rng_), ranges::end(rng_)};
        }

    public:
        group_by_view() = default;
        constexpr group_by_view(Rng rng, Fun fun)
          : rng_(std::move(rng))
          , fun_(std::move(fun))
        {}
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Fun)(requires copy_constructible<Fun>)
        group_by_view(Rng &&, Fun)
            ->group_by_view<views::all_t<Rng>, Fun>;
#endif

    namespace views
    {
        struct group_by_fn
        {
        private:
            friend view_access;
            template<typename Fun>
            static constexpr auto bind(group_by_fn group_by, Fun fun)
            {
                return make_pipeable(bind_back(group_by, std::move(fun)));
            }

        public:
            template<typename Rng, typename Fun>
            constexpr auto operator()(Rng && rng, Fun fun) const
                -> CPP_ret(group_by_view<all_t<Rng>, Fun>)( //
                    requires viewable_range<Rng> && forward_range<Rng> &&
                        indirect_relation<Fun, iterator_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(fun)};
            }
        };

        /// \relates group_by_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<group_by_fn>, group_by)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::group_by_view)

#endif
