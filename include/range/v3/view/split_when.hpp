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

#ifndef RANGES_V3_VIEW_SPLIT_WHEN_HPP
#define RANGES_V3_VIEW_SPLIT_WHEN_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take_while.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    template<typename Rng, typename Fun>
    struct split_when_view
      : view_facade<
            split_when_view<Rng, Fun>,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        friend range_access;
        Rng rng_;
        semiregular_t<Fun> fun_;

        template<bool IsConst>
        struct cursor
        {
        private:
            friend range_access;
            friend split_when_view;
            friend struct cursor<!IsConst>;
            bool zero_;
            using CRng = meta::const_if_c<IsConst, Rng>;
            iterator_t<CRng> cur_;
            sentinel_t<CRng> last_;
            using fun_ref_t = semiregular_ref_or_val_t<Fun, IsConst>;
            fun_ref_t fun_;

            struct search_pred
            {
                bool zero_;
                iterator_t<CRng> first_;
                sentinel_t<CRng> last_;
                fun_ref_t fun_;
                bool operator()(iterator_t<CRng> cur) const
                {
                    return (zero_ && cur == first_) || (cur != last_ && !invoke(fun_, cur, last_).first);
                }
            };
            using reference_ =
                indirect_view<take_while_view<iota_view<iterator_t<CRng>>, search_pred>>;
            reference_ read() const
            {
                return reference_{{view::iota(cur_), {zero_, cur_, last_, fun_}}};
            }
            void next()
            {
                RANGES_EXPECT(cur_ != last_);
                // If the last match consumed zero elements, bump the position.
                advance(cur_, (int)zero_, last_);
                zero_ = false;
                for(; cur_ != last_; ++cur_)
                {
                    auto p = invoke(fun_, cur_, last_);
                    if(p.first)
                    {
                        zero_ = (cur_ == p.second);
                        cur_ = p.second;
                        return;
                    }
                }
            }
            bool equal(default_sentinel_t) const
            {
                return cur_ == last_;
            }
            bool equal(cursor const &that) const
            {
                return cur_ == that.cur_;
            }
            cursor(fun_ref_t fun, iterator_t<CRng> first, sentinel_t<CRng> last)
              : cur_(first), last_(last), fun_(fun)
            {
                // For skipping an initial zero-length match
                auto p = invoke(fun, first, ranges::next(first));
                zero_ = p.first && first == p.second;
            }
        public:
            cursor() = default;
            template<bool Other>
            CPP_ctor(cursor)(cursor<Other> that)(
                requires IsConst && (!Other))
              : cursor{std::move(that.cur_), std::move(that.last_), std::move(that.fun_)}
            {}
        };
        cursor<false> begin_cursor()
        {
            return {fun_, ranges::begin(rng_), ranges::end(rng_)};
        }
        template<bool Const = true>
        auto begin_cursor() const ->
            CPP_ret(cursor<Const>)(
                requires Const && Range<meta::const_if_c<Const, Rng>> &&
                    Invocable<
                        Fun const &,
                        iterator_t<meta::const_if_c<Const, Rng>>,
                        sentinel_t<meta::const_if_c<Const, Rng>>>)
        {
            return {fun_, ranges::begin(rng_), ranges::end(rng_)};
        }
    public:
        split_when_view() = default;
        split_when_view(Rng rng, Fun fun)
          : rng_(std::move(rng))
          , fun_(std::move(fun))
        {}
    };

    namespace view
    {
        struct split_when_fn
        {
        private:
            friend view_access;
            template<typename T>
            static auto bind(split_when_fn split_when, T &&t)
            {
                return make_pipeable(std::bind(split_when, std::placeholders::_1,
                    bind_forward<T>(t)));
            }
            template<typename Pred>
            struct predicate_pred
            {
                semiregular_t<Pred> pred_;

                template<typename I, typename S>
                auto operator()(I cur, S end) const ->
                    CPP_ret(std::pair<bool, I>)(
                        requires Sentinel<S, I>)
                {
                    auto where = ranges::find_if_not(cur, end, std::ref(pred_));
                    return {cur != where, where};
                }
            };
        public:
            template<typename Rng, typename Fun>
            auto operator()(Rng &&rng, Fun fun) const ->
                CPP_ret(split_when_view<all_t<Rng>, Fun>)(
                    requires ViewableRange<Rng> && ForwardRange<Rng> &&
                        Invocable<Fun&, iterator_t<Rng>, sentinel_t<Rng>> &&
                        Invocable<Fun&, iterator_t<Rng>, iterator_t<Rng>> &&
                        CopyConstructible<Fun> &&
                        ConvertibleTo<
                            invoke_result_t<Fun&, iterator_t<Rng>, sentinel_t<Rng>>,
                            std::pair<bool, iterator_t<Rng>>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(fun)};
            }
            template<typename Rng, typename Fun>
            auto operator()(Rng &&rng, Fun fun) const ->
                CPP_ret(split_when_view<all_t<Rng>, predicate_pred<Fun>>)(
                    requires ViewableRange<Rng> && ForwardRange<Rng> &&
                        Predicate<Fun const&, range_reference_t<Rng>> &&
                        CopyConstructible<Fun>)
            {
                return {all(static_cast<Rng &&>(rng)),
                        predicate_pred<Fun>{std::move(fun)}};
            }
        };

        /// \relates split_when_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<split_when_fn>, split_when)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::split_when_view)

#endif
