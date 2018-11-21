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

#ifndef RANGES_V3_VIEW_SPLIT_HPP
#define RANGES_V3_VIEW_SPLIT_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/algorithm/find_if_not.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct split_view
          : view_facade<
                split_view<Rng, Fun>,
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
                friend split_view;
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
                bool equal(default_sentinel) const
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
                    requires IsConst && !Other)
                  : cursor{std::move(that.cur_), std::move(that.last_), std::move(that.fun_)}
                {}
            };
            cursor<false> begin_cursor()
            {
                return {fun_, ranges::begin(rng_), ranges::end(rng_)};
            }
            template<typename CRng = Rng const>
            auto begin_cursor() const -> CPP_ret(cursor<true>)(
                requires Range<CRng> &&
                    Invocable<Fun const&, iterator_t<CRng>, sentinel_t<CRng>>)
            {
                return {fun_, ranges::begin(rng_), ranges::end(rng_)};
            }
        public:
            split_view() = default;
            split_view(Rng rng, Fun fun)
              : rng_(std::move(rng))
              , fun_(std::move(fun))
            {}
        };

        namespace view
        {
            CPP_def
            (
                template(typename Rng, typename Fun)
                concept SplitOnFunction,
                    ForwardRange<Rng> &&
                    Invocable<Fun&, iterator_t<Rng>, sentinel_t<Rng>> &&
                    Invocable<Fun&, iterator_t<Rng>, iterator_t<Rng>> &&
                    CopyConstructible<Fun> &&
                    ConvertibleTo<
                        invoke_result_t<Fun&, iterator_t<Rng>, sentinel_t<Rng>>,
                        std::pair<bool, iterator_t<Rng>>>
            );
            CPP_def
            (
                template(typename Rng, typename Fun)
                concept SplitOnPredicate,
                    ForwardRange<Rng> &&
                    Predicate<Fun const&, range_reference_t<Rng>> &&
                    CopyConstructible<Fun>
            );
            CPP_def
            (
                template(typename Rng)
                concept SplitOnElement,
                    ForwardRange<Rng> &&
                    Regular<range_value_t<Rng>>
            );
            CPP_def
            (
                template(typename Rng, typename Sub)
                concept SplitOnSubRange,
                    ForwardRange<Rng> &&
                    ForwardRange<Sub> &&
                    EqualityComparableWith<range_value_t<Rng>, range_value_t<Sub>>
            );

            struct split_fn
            {
            private:
                friend view_access;
                template<typename T>
                static auto bind(split_fn split, T &&t)
                {
                    return make_pipeable(std::bind(split, std::placeholders::_1,
                        bind_forward<T>(t)));
                }
                template<typename Rng, typename Pred>
                struct predicate_pred
                {
                    semiregular_t<Pred> pred_;

                    template<typename S>
                    auto operator()(iterator_t<Rng> cur, S end) const ->
                        CPP_ret(std::pair<bool, iterator_t<Rng>>)(
                            requires Sentinel<S, iterator_t<Rng>>)
                    {
                        auto where = ranges::find_if_not(cur, end, std::ref(pred_));
                        return std::pair<bool, iterator_t<Rng>>{cur != where, where};
                    }
                };
                template<typename Rng>
                struct element_pred
                {
                    range_value_t<Rng> val_;

                    template<typename S>
                    auto operator()(iterator_t<Rng> cur, S end) const ->
                        CPP_ret(std::pair<bool, iterator_t<Rng>>)(
                            requires Sentinel<S, iterator_t<Rng>>)
                    {
                        RANGES_EXPECT(cur != end);
                        bool const match = *cur == val_;
                        if (match) ++cur;
                        return std::pair<bool, iterator_t<Rng>>{match, cur};
                    }
                };
                template<typename Rng, typename Sub>
                struct subrange_pred
                {
                    all_t<Sub> sub_;
                    range_difference_t<Sub> len_;

                    subrange_pred() = default;
                    subrange_pred(Sub &&sub)
                      : sub_(all(static_cast<Sub &&>(sub))), len_(distance(sub_))
                    {}
                    template<typename S>
                    auto operator()(iterator_t<Rng> cur, S end) const ->
                        CPP_ret(std::pair<bool, iterator_t<Rng>>)(
                            requires Sentinel<S, iterator_t<Rng>>)
                    {
                        using P = std::pair<bool, iterator_t<Rng>>;
                        RANGES_EXPECT(cur != end);
                        if((bool) SizedSentinel<S, iterator_t<Rng>> && distance(cur, end) < len_)
                            return P{false, cur};
                        auto pat_cur = ranges::begin(sub_);
                        auto pat_end = ranges::end(sub_);
                        for(;; ++cur, ++pat_cur)
                        {
                            if(pat_cur == pat_end)
                                return P{true, cur};
                            if(cur == end || !(*cur == *pat_cur))
                                return P{false, cur};
                        }
                    }
                };
            public:
                template<typename Rng, typename Fun>
                auto operator()(Rng &&rng, Fun fun) const ->
                    CPP_ret(split_view<all_t<Rng>, Fun>)(
                        requires SplitOnFunction<Rng, Fun>)
                {
                    return {all(static_cast<Rng &&>(rng)), std::move(fun)};
                }
                template<typename Rng, typename Fun>
                auto operator()(Rng &&rng, Fun fun) const ->
                    CPP_ret(split_view<all_t<Rng>, predicate_pred<Rng, Fun>>)(
                        requires SplitOnPredicate<Rng, Fun>)
                {
                    return {all(static_cast<Rng &&>(rng)), predicate_pred<Rng, Fun>{std::move(fun)}};
                }
                template<typename Rng>
                auto operator()(Rng &&rng, range_value_t<Rng> val) const ->
                    CPP_ret(split_view<all_t<Rng>, element_pred<Rng>>)(
                        requires SplitOnElement<Rng>)
                {
                    return {all(static_cast<Rng &&>(rng)), {std::move(val)}};
                }
                template<typename Rng, typename Sub>
                auto operator()(Rng &&rng, Sub &&sub) const ->
                    CPP_ret(split_view<all_t<Rng>, subrange_pred<Rng, Sub>>)(
                        requires SplitOnSubRange<Rng, Sub>)
                {
                    return {all(static_cast<Rng &&>(rng)), {static_cast<Sub &&>(sub)}};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T>
                auto operator()(Rng &&, T &&) const volatile ->
                    CPP_ret(void)(
                        requires not ConvertibleTo<T, range_value_t<Rng>>)
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which view::split operates must be a model of the "
                        "ForwardRange concept.");
                    CPP_assert_msg(ConvertibleTo<T, range_value_t<Rng>>,
                        "The delimiter argument to view::split must be one of the following: "
                        "(1) A single element of the range's value type, where the value type is a "
                        "model of the Regular concept, "
                        "(2) A ForwardRange whose value type is EqualityComparableWith to the input "
                        "range's value type, "
                        "(3) A Predicate that is callable with one argument of the range's reference "
                        "type, or "
                        "(4) A Callable that accepts two arguments, the range's iterator "
                        "and sentinel, and that returns a std::pair<bool, I> where I is the "
                        "input range's iterator type.");
                }
            #endif
            };

            /// \relates split_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<split_fn>, split)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::split_view)

#endif
