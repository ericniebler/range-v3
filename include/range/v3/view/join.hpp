/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_JOIN_HPP
#define RANGES_V3_VIEW_JOIN_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range_for.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/variant.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/iterator/default_sentinel.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        // Compute the cardinality of a joined range
        constexpr cardinality join_cardinality_(
            cardinality Outer,
            cardinality Inner,
            cardinality Joiner = static_cast<cardinality>(0)) noexcept
        {
            return Outer == infinite || Inner == infinite || (Joiner == infinite && Outer != 0 && Outer != 1) ?
                infinite :
                Outer == unknown || Inner == unknown || (Joiner == unknown && Outer != 0 && Outer != 1) ?
                    unknown :
                    Outer == finite || Inner == finite || (Joiner == finite && Outer != 0 && Outer != 1) ?
                        finite :
                        static_cast<cardinality>(Outer * Inner + (Outer == 0 ? 0 : (Outer - 1) * Joiner));
        }

        template<typename Range>
        constexpr cardinality join_cardinality() noexcept
        {
            return detail::join_cardinality_(
                range_cardinality<Range>::value,
                range_cardinality<range_reference_t<Range>>::value
            );
        }

        template<typename Range, typename JoinRange>
        constexpr cardinality join_cardinality() noexcept
        {
            return detail::join_cardinality_(
                range_cardinality<Range>::value,
                range_cardinality<range_reference_t<Range>>::value,
                range_cardinality<JoinRange>::value
            );
        }

        template<typename Inner>
        struct store_inner_
        {
            view::all_t<Inner> inner_ = view::all_t<Inner>();

            constexpr view::all_t<Inner> &update_inner_(Inner &&inner)
            {
                return (inner_ = view::all(static_cast<Inner &&>(inner)));
            }
            constexpr view::all_t<Inner> &get_inner_(ignore_t) noexcept
            {
                return inner_;
            }
        };

        struct pass_thru_inner_
        {
            // Intentionally promote xvalues to lvalues here:
            template<typename Inner>
            static constexpr Inner &update_inner_(Inner &&inner) noexcept
            {
                return inner;
            }
            template<typename OuterIt>
            static constexpr decltype(auto) get_inner_(OuterIt &&outer_it)
            {
                return *outer_it;
            }
        };

        template<typename Rng>
        using join_view_inner =
            if_then_t<
                !std::is_reference<range_reference_t<Rng>>::value,
                store_inner_<range_reference_t<Rng>>,
                pass_thru_inner_>;

        CPP_def
        (
            template(typename I)
            concept HasMemberArrow_,
                requires (I i)
                (
                    i.operator->()
                )
        );

        CPP_def
        (
            template(typename I)
            concept HasArrow_,
                InputIterator<I> && (std::is_pointer<I>::value || HasMemberArrow_<I>)
        );
    }
    /// \endcond

    /// \addtogroup group-views
    /// @{

    // Join a range of ranges
    template<typename Rng>
    struct RANGES_EMPTY_BASES join_view
      : view_facade<join_view<Rng>, detail::join_cardinality<Rng>()>
      , private detail::join_view_inner<Rng>
    {
        CPP_assert(InputView<Rng>);
        CPP_assert(InputRange<range_reference_t<Rng>>);
        CPP_assert(ViewableRange<range_reference_t<Rng>>);

        join_view() = default;
        explicit join_view(Rng rng)
          : outer_(view::all(std::move(rng)))
        {}
        // Not to spec
        CPP_member
        static constexpr auto size() -> CPP_ret(std::size_t)(
            requires (detail::join_cardinality<Rng>() >= 0))
        {
            return static_cast<std::size_t>(detail::join_cardinality<Rng>());
        }
        // Not to spec
        CPP_member
        constexpr /*c++14*/ auto CPP_fun(size)() (
            requires detail::join_cardinality<Rng>() < 0 &&
                range_cardinality<Rng>::value >= 0 &&
                ForwardRange<Rng> &&
                SizedRange<range_reference_t<Rng>>)
        {
            range_size_t<range_reference_t<Rng>> n = 0;
            RANGES_FOR(auto &&inner, outer_)
                n += ranges::size(inner);
            return n;
        }
        // // ericniebler/stl2#605
        constexpr Rng base() const
        {
            return outer_;
        }
    private:
        friend range_access;
        Rng outer_{};

        template<bool Const>
        struct cursor
        {
        private:
            using Parent = detail::if_then_t<Const, join_view const, join_view>;
            using COuter = detail::if_then_t<Const, Rng const, Rng>;
            using CInner = range_reference_t<COuter>;
            using ref_is_glvalue = std::is_reference<CInner>;

            Parent *rng_ = nullptr;
            iterator_t<COuter> outer_it_{};
            iterator_t<CInner> inner_it_{};

            void satisfy()
            {
                for(; outer_it_ != ranges::end(rng_->outer_); ++outer_it_)
                {
                    auto &inner = rng_->update_inner_(*outer_it_);
                    inner_it_ = ranges::begin(inner);
                    if(inner_it_ != ranges::end(inner))
                        return;
                }
                if RANGES_CONSTEXPR_IF (ref_is_glvalue::value)
                    inner_it_ = iterator_t<CInner>();
            }
        public:
            using single_pass =
                meta::bool_<
                    SinglePass<iterator_t<COuter>> ||
                    SinglePass<iterator_t<CInner>> ||
                    !ref_is_glvalue::value>;
            cursor() = default;
            template<typename BeginOrEnd>
            constexpr cursor(Parent &rng, BeginOrEnd begin_or_end)
              : rng_{&rng}
              , outer_it_(begin_or_end(rng.outer_))
            {
                satisfy();
            }
            template<bool Other>
            constexpr CPP_ctor(cursor)(cursor<Other> that)(
                requires Const && !Other &&
                    ConvertibleTo<iterator_t<Rng>, iterator_t<COuter>> &&
                    ConvertibleTo<iterator_t<range_reference_t<Rng>>, iterator_t<CInner>>)
              : rng_(that.rng_)
              , outer_it_(std::move(that.outer_it_))
              , inner_it_(std::move(that.inner_it_))
            {}
            CPP_member
            constexpr auto arrow() -> CPP_ret(iterator_t<CInner>)(
                requires detail::HasArrow_<iterator_t<CInner>>)
            {
                return inner_it_;
            }
            constexpr bool equal(default_sentinel_t) const
            {
                return outer_it_ == ranges::end(rng_->outer_);
            }
            CPP_member
            constexpr auto equal(cursor const &that) const -> CPP_ret(bool)(
                requires ref_is_glvalue::value &&
                    EqualityComparable<iterator_t<COuter>> &&
                    EqualityComparable<iterator_t<CInner>>)
            {
                return outer_it_ == that.outer_it_ && inner_it_ == that.inner_it_;
            }
            constexpr /*c++14*/ void next()
            {
                auto &&inner_rng = rng_->get_inner_(outer_it_);
                if(++inner_it_ == ranges::end(inner_rng))
                {
                    ++outer_it_;
                    satisfy();
                }
            }
            CPP_member
            constexpr /*c++14*/ auto prev() -> CPP_ret(void)(
                requires ref_is_glvalue::value &&
                    BidirectionalRange<COuter> &&
                    BidirectionalRange<CInner> &&
                    CommonRange<CInner>) // ericniebler/stl2#606
            {
                if(outer_it_ == ranges::end(rng_->outer_))
                    inner_it_ = ranges::end(*--outer_it_);
                while(inner_it_ == ranges::begin(*outer_it_))
                    inner_it_ = ranges::end(*--outer_it_);
                --inner_it_;
            }
            constexpr auto CPP_auto_fun(read)() (const)
            (
                return *inner_it_
            )
            constexpr auto CPP_auto_fun(move)() (const)
            (
                return iter_move(inner_it_)
            )
        };
        static constexpr bool use_const_always() noexcept
        {
            return simple_view<Rng>() && std::is_reference<range_reference_t<Rng>>::value;
        }
        struct end_cursor_fn
        {
            constexpr auto operator()(join_view &this_, std::true_type) const
            {
                return cursor<use_const_always()>{this_, ranges::end};
            }
            constexpr auto operator()(join_view &, std::false_type) const
            {
                return default_sentinel_t{};
            }
        };
        struct cend_cursor_fn
        {
            constexpr auto operator()(join_view const &this_, std::true_type) const
            {
                return cursor<true>{this_, ranges::end};
            }
            constexpr auto operator()(join_view const &, std::false_type) const
            {
                return default_sentinel_t{};
            }
        };

        constexpr /*c++14*/ cursor<use_const_always()> begin_cursor()
        {
            return {*this, ranges::begin};
        }

        template<bool Const = true>
        constexpr auto begin_cursor() const ->
            CPP_ret(cursor<Const>)(
                requires Const &&
                    InputRange<meta::const_if_c<Const, Rng>> &&
                    std::is_reference<
                        range_reference_t<meta::const_if_c<Const, Rng>>>::value)
        {
            return {*this, ranges::begin};
        }

        constexpr /*c++14*/ auto end_cursor()
        {
            using cond = meta::bool_<
                std::is_reference<range_reference_t<Rng>>::value &&
                ForwardRange<Rng> && ForwardRange<range_reference_t<Rng>> &&
                CommonRange<Rng> && CommonRange<range_reference_t<Rng>>>;
            return end_cursor_fn{}(*this, cond{});
        }

        template<bool Const = true>
        constexpr auto CPP_fun(end_cursor)() (const
            requires Const && InputRange<meta::const_if_c<Const, Rng>> &&
                std::is_reference<
                    range_reference_t<meta::const_if_c<Const, Rng>>>::value)
        {
            using CRng = meta::const_if_c<Const, Rng>;
            using cond = meta::bool_<
                std::is_reference<range_reference_t<CRng>>::value &&
                ForwardRange<CRng> && ForwardRange<range_reference_t<CRng>> &&
                CommonRange<CRng> && CommonRange<range_reference_t<CRng>>>;
            return cend_cursor_fn{}(*this, cond{});
        }
    };

    // Join a range of ranges, inserting a range of values between them.
    // TODO: Support const iteration when range_reference_t<Rng> is a true reference.
    template<typename Rng, typename ValRng>
    struct join_with_view
      : view_facade<join_with_view<Rng, ValRng>, detail::join_cardinality<Rng, ValRng>()>
    {
        CPP_assert(InputRange<Rng>);
        CPP_assert(InputRange<range_reference_t<Rng>>);
        CPP_assert(ForwardRange<ValRng>);
        CPP_assert(Common<range_value_t<range_reference_t<Rng>>, range_value_t<ValRng>>);
        CPP_assert(Semiregular<common_type_t<
            range_value_t<range_reference_t<Rng>>,
            range_value_t<ValRng>>>);

        join_with_view() = default;
        join_with_view(Rng rng, ValRng val)
          : outer_(view::all(std::move(rng)))
          , val_(view::all(std::move(val)))
        {}
        CPP_member
        static constexpr auto size() -> CPP_ret(std::size_t)(
            requires detail::join_cardinality<Rng, ValRng>() >= 0)
        {
            return static_cast<std::size_t>(detail::join_cardinality<Rng, ValRng>());
        }
        CPP_member
        auto CPP_fun(size)() (const
            requires detail::join_cardinality<Rng, ValRng>() < 0 &&
                range_cardinality<Rng>::value >= 0 && ForwardRange<Rng> &&
                SizedRange<range_reference_t<Rng>> && SizedRange<ValRng>)
        {
            range_size_t<range_reference_t<Rng>> n = 0;
            RANGES_FOR(auto &&inner, outer_)
                n += ranges::size(inner);
            return n + (range_cardinality<Rng>::value == 0
                ? 0
                : ranges::size(val_) * (range_cardinality<Rng>::value - 1));
        }
    private:
        friend range_access;
        using Outer = view::all_t<Rng>;
        using Inner = view::all_t<range_reference_t<Outer>>;

        Outer outer_{};
        Inner inner_{};
        view::all_t<ValRng> val_{};

        class cursor
        {
            join_with_view* rng_ = nullptr;
            iterator_t<Outer> outer_it_{};
            variant<iterator_t<ValRng>, iterator_t<Inner>> cur_{};

            void satisfy()
            {
                while(true)
                {
                    if(cur_.index() == 0)
                    {
                        if(ranges::get<0>(cur_) != ranges::end(rng_->val_))
                            break;
                        rng_->inner_ = view::all(*outer_it_);
                        ranges::emplace<1>(cur_, ranges::begin(rng_->inner_));
                    }
                    else
                    {
                        if(ranges::get<1>(cur_) != ranges::end(rng_->inner_))
                            break;
                        if(++outer_it_ == ranges::end(rng_->outer_))
                            break;
                        ranges::emplace<0>(cur_, ranges::begin(rng_->val_));
                    }
                }
            }
        public:
            using value_type = common_type_t<
                range_value_t<Inner>, range_value_t<ValRng>>;
            using reference = common_reference_t<
                range_reference_t<Inner>, range_reference_t<ValRng>>;
            using rvalue_reference = common_reference_t<
                range_rvalue_reference_t<Inner>, range_rvalue_reference_t<ValRng>>;
            using single_pass = std::true_type;
            cursor() = default;
            cursor(join_with_view &rng)
              : rng_{&rng}
              , outer_it_(ranges::begin(rng.outer_))
            {
                if(outer_it_ != ranges::end(rng_->outer_))
                {
                    rng.inner_ = view::all(*outer_it_);
                    ranges::emplace<1>(cur_, ranges::begin(rng.inner_));
                    satisfy();
                }
            }
            bool equal(default_sentinel_t) const
            {
                return outer_it_ == ranges::end(rng_->outer_);
            }
            void next()
            {
                // visit(cur_, [](auto& it){ ++it; });
                if(cur_.index() == 0)
                {
                    auto& it = ranges::get<0>(cur_);
                    RANGES_ASSERT(it != ranges::end(rng_->val_));
                    ++it;
                }
                else
                {
                    auto& it = ranges::get<1>(cur_);
                    RANGES_ASSERT(it != ranges::end(rng_->inner_));
                    ++it;
                }
                satisfy();
            }
            reference read() const
            {
                // return visit(cur_, [](auto& it) -> reference { return *it; });
                if(cur_.index() == 0)
                    return *ranges::get<0>(cur_);
                else
                    return *ranges::get<1>(cur_);
            }
            rvalue_reference move() const
            {
                // return visit(cur_, [](auto& it) -> rvalue_reference { return iter_move(it); });
                if(cur_.index() == 0)
                    return iter_move(ranges::get<0>(cur_));
                else
                    return iter_move(ranges::get<1>(cur_));
            }
        };
        cursor begin_cursor()
        {
            return {*this};
        }
    };

    namespace view
    {
        /// \cond
        // Don't forget to update view::for_each whenever this set
        // of concepts changes
        CPP_def
        (
            template(typename Rng)
            concept JoinableRange,
                ViewableRange<Rng> && InputRange<Rng> &&
                InputRange<range_reference_t<Rng>> &&
                ViewableRange<range_reference_t<Rng>>
        );

        CPP_def
        (
            template(typename Rng, typename ValRng)
            concept JoinableWithRange,
                JoinableRange<Rng> &&
                ViewableRange<ValRng> && ForwardRange<ValRng> &&
                Common<range_value_t<ValRng>, range_value_t<range_reference_t<Rng>>> &&
                Semiregular<
                    common_type_t<
                        range_value_t<ValRng>,
                        range_value_t<range_reference_t<Rng>>>> &&
                CommonReference<
                    range_reference_t<ValRng>,
                    range_reference_t<range_reference_t<Rng>>> &&
                CommonReference<
                    range_rvalue_reference_t<ValRng>,
                    range_rvalue_reference_t<range_reference_t<Rng>>>
        );
        /// \endcond

        struct join_fn
        {
        private:
           friend view_access;
           template<typename T>
           static auto CPP_fun(bind)(join_fn join, T &&t)(
               requires (!JoinableRange<T>))
           {
               return make_pipeable(std::bind(join, std::placeholders::_1, bind_forward<T>(t)));
           }
           template<typename Rng>
           using inner_value_t = range_value_t<range_reference_t<Rng>>;
        public:
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(join_view<all_t<Rng>>)(
                    requires JoinableRange<Rng>)
            {
                return join_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
            template<typename Rng>
            auto operator()(Rng &&rng, inner_value_t<Rng> v) const ->
                CPP_ret(join_with_view<all_t<Rng>, single_view<inner_value_t<Rng>>>)(
                    requires JoinableWithRange<Rng, single_view<inner_value_t<Rng>>>)
            {
                return {all(static_cast<Rng &&>(rng)), single(std::move(v))};
            }
            template<typename Rng, typename ValRng>
            auto operator()(Rng &&rng, ValRng &&val) const ->
                CPP_ret(join_with_view<all_t<Rng>, all_t<ValRng>>)(
                    requires JoinableWithRange<Rng, ValRng>)
            {
                return {all(static_cast<Rng &&>(rng)), all(static_cast<ValRng &&>(val))};
            }
        };

        /// \relates join_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<join_fn>, join)
    }
    /// @}

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng)(
        requires view::JoinableRange<Rng>)
    explicit join_view(Rng &&) -> join_view<view::all_t<Rng>>;

    CPP_template(typename Rng, typename ValRng)(
        requires view::JoinableWithRange<Rng, ValRng>)
    explicit join_with_view(Rng &&, ValRng &&) ->
        join_with_view<view::all_t<Rng>, view::all_t<ValRng>>;
#endif

}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::join_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::join_with_view)

#endif
