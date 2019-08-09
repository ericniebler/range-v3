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

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/mismatch.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    /// \cond
    namespace detail
    {
        // clang-format off
        CPP_def
        (
            template(typename R)
            concept tiny_range,
                SizedRange<R> &&
                Type<std::integral_constant<
                    decltype(std::remove_reference_t<R>::size()),
                    std::remove_reference_t<R>::size()>> &&
                (std::remove_reference_t<R>::size() <= 1)
        );
        // clang-format on
    } // namespace detail

    template<typename V, typename Pattern>
#if CPP_CXX_CONCEPTS
        requires InputRange<V> && ForwardRange<Pattern> && View<V> && View<
            Pattern> && IndirectlyComparable<iterator_t<V>, iterator_t<Pattern>,
                                             ranges::equal_to> &&
        (ForwardRange<V> || detail::tiny_range<Pattern>)
#endif
            struct split_view;

    namespace detail
    {
        struct there
        {
            template<typename T>
            static decltype(auto) current_(T & t) noexcept
            {
                return (t.curr_);
            }
        };

        template<typename It>
        struct here
        {
            It curr_ = It();
            It & current_(ignore_t) noexcept
            {
                return curr_;
            }
            It const & current_(ignore_t) const noexcept
            {
                return curr_;
            }
        };

        template<bool>
        struct here_or_there_
        {
            template<typename>
            using invoke = there;
        };

        template<>
        struct here_or_there_<true>
        {
            template<typename It>
            using invoke = here<It>;
        };

        template<typename It>
        using split_view_base = meta::invoke<here_or_there_<!ForwardIterator<It>>, It>;

        template<typename JoinView, bool Const>
        struct split_outer_iterator;

        template<typename JoinView, bool Const>
        struct split_inner_iterator;

        template<typename V, typename Pattern, bool Const>
        struct split_inner_iterator<split_view<V, Pattern>, Const>
        {
        private:
            using Outer = split_outer_iterator<split_view<V, Pattern>, Const>;
            using Base = meta::const_if_c<Const, V>;
            using BaseIterCategory =
                typename std::iterator_traits<iterator_t<Base>>::iterator_category;
            Outer i_ = Outer();
            bool incremented_ = false;
            constexpr decltype(auto) current_() noexcept
            {
                return i_.current_();
            }
            constexpr decltype(auto) current_() const noexcept
            {
                return i_.current_();
            }
            constexpr bool done_() const
            {
                auto cur = current_();
                auto end = ranges::end(i_.parent_->base_);
                if(cur == end)
                    return true;
                auto pcur = ranges::begin(i_.parent_->pattern_);
                auto pend = ranges::end(i_.parent_->pattern_);
                if(pcur == pend)
                    return incremented_;
                do
                {
                    if(*cur != *pcur)
                        return false;
                    if(++pcur == pend)
                        return true;
                } while(++cur != end);
                return false;
            }
#if RANGES_CXX_IF_CONSTEXPR < RANGES_CXX_IF_CONSTEXPR_17
            constexpr void pre_inc(std::true_type) // Forward
            {
                ++current_();
            }
            constexpr void pre_inc(std::false_type) // Input
            {
                if(Pattern::size() != 0)
                    ++current_();
            }
            constexpr split_inner_iterator post_inc(std::true_type) // Forward
            {
                auto tmp = *this;
                pre_inc(std::true_type{});
                return tmp;
            }
            constexpr void post_inc(std::false_type) // Input
            {
                pre_inc(std::false_type{});
            }
#endif
        public:
            using iterator_concept = typename Outer::iterator_concept;
            using iterator_category =
                if_then_t<DerivedFrom<BaseIterCategory, std::forward_iterator_tag>,
                          std::forward_iterator_tag, std::input_iterator_tag>;
            using value_type = range_value_t<Base>;
            using difference_type = range_difference_t<Base>;
            using reference = range_reference_t<Base>;        // Not to spec
            using pointer = iter_pointer_t<iterator_t<Base>>; // Not to spec

            split_inner_iterator() = default;

            constexpr explicit split_inner_iterator(Outer i)
              : i_(std::move(i))
            {}

            constexpr decltype(auto) operator*() const
            {
                return *current_();
            }

            constexpr split_inner_iterator & operator++()
            {
                incremented_ = true;
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
                if constexpr(!ForwardRange<Base>)
                    if constexpr(Pattern::size() == 0)
                        return *this;
                ++current_();
#else
                pre_inc(meta::bool_<ForwardRange<Base>>{});
#endif
                return *this;
            }

            constexpr decltype(auto) operator++(int)
            {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
                if constexpr(ForwardRange<V>)
                {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
                else
                    ++*this;
#else
                return post_inc(meta::bool_<ForwardRange<V>>{});
#endif
            }

            CPP_broken_friend_member
            friend constexpr auto operator==(split_inner_iterator const & x,
                                             split_inner_iterator const & y)
                -> CPP_broken_friend_ret(bool)( //
                    requires ForwardRange<Base>)
            {
                return x.i_.curr_ == y.i_.curr_;
            }
            CPP_broken_friend_member
            friend constexpr auto operator!=(split_inner_iterator const & x,
                                             split_inner_iterator const & y)
                -> CPP_broken_friend_ret(bool)( //
                    requires ForwardRange<Base>)
            {
                return x.i_.curr_ != y.i_.curr_;
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator==(split_inner_iterator const & x,
                                             default_sentinel_t)
            {
                return x.done_();
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator==(default_sentinel_t,
                                             split_inner_iterator const & x)
            {
                return x.done_();
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator!=(split_inner_iterator const & x,
                                             default_sentinel_t)
            {
                return !x.done_();
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator!=(default_sentinel_t,
                                             split_inner_iterator const & x)
            {
                return !x.done_();
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr decltype(auto) iter_move(
                split_inner_iterator const &
                    i) noexcept(noexcept(ranges::iter_move(i.current_())))
            {
                return ranges::iter_move(i.current_());
            }
            CPP_broken_friend_member
            friend constexpr auto iter_swap(
                split_inner_iterator const & x,
                split_inner_iterator const &
                    y) noexcept(noexcept(ranges::iter_swap(x.current_(), y.current_())))
                -> CPP_broken_friend_ret(void)( //
                    requires IndirectlySwappable<iterator_t<Base>>)
            {
                ranges::iter_swap(x.current_(), y.current_());
            }
        };

        template<typename It>
        using split_outer_iterator_base =
            meta::invoke<here_or_there_<ForwardIterator<It>>, It>;

        template<typename JoinView, bool Const>
        struct split_outer_iterator;

        template<typename V, typename Pattern, bool Const>
        struct split_outer_iterator<split_view<V, Pattern>, Const>
          : split_outer_iterator_base<iterator_t<meta::const_if_c<Const, V>>>
        {
        private:
            friend struct split_inner_iterator<split_view<V, Pattern>, Const>;
            using Parent = meta::const_if_c<Const, split_view<V, Pattern>>;
            using Base = meta::const_if_c<Const, V>;
            using Current = split_outer_iterator_base<iterator_t<Base>>;

            Parent * parent_ = nullptr;
            constexpr decltype(auto) current_() noexcept
            {
                return parent_->current_(*this);
            }
            constexpr decltype(auto) current_() const noexcept
            {
                return parent_->current_(*this);
            }
            constexpr decltype(auto) base_() const noexcept
            {
                return (parent_->base_);
            }
#if RANGES_CXX_IF_CONSTEXPR < RANGES_CXX_IF_CONSTEXPR_17
            constexpr split_outer_iterator post_inc(std::true_type) // Forward
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }
            constexpr void post_inc(std::false_type) // Input
            {
                ++*this;
            }
#endif

        public:
            using iterator_concept =
                if_then_t<ForwardRange<Base>, std::forward_iterator_tag,
                          std::input_iterator_tag>;
            using iterator_category = std::input_iterator_tag;
            struct value_type : view_interface<value_type>
            {
            private:
                split_outer_iterator i_ = split_outer_iterator();

            public:
                value_type() = default;
                constexpr explicit value_type(split_outer_iterator i)
                  : i_(std::move(i))
                {}
                constexpr split_inner_iterator<split_view<V, Pattern>, Const> begin()
                    const
                {
                    return split_inner_iterator<split_view<V, Pattern>, Const>(i_);
                }
                constexpr default_sentinel_t end() const
                {
                    return default_sentinel;
                }
            };
            using difference_type = range_difference_t<Base>;
            using reference = value_type; // Not to spec
            using pointer = value_type *; // Not to spec

            split_outer_iterator() = default;

            CPP_member
            constexpr explicit CPP_ctor(split_outer_iterator)(Parent & parent)( //
                requires(!ForwardRange<Base>))
              : parent_(&parent)
            {}

            CPP_member
            constexpr CPP_ctor(split_outer_iterator)(Parent & parent,
                                                     iterator_t<Base> current)( //
                requires ForwardRange<Base>)
              : Current{std::move(current)}
              , parent_(&parent)
            {}

            CPP_template(bool Other)( //
                requires Const && (!Other) &&
                ConvertibleTo<iterator_t<V>, iterator_t<Base>>) //
                constexpr split_outer_iterator(
                    split_outer_iterator<split_view<V, Pattern>, Other> i)
              : Current{std::move(i.curr_)}
              , parent_(i.parent_)
            {}

            constexpr value_type operator*() const
            {
                return value_type{*this};
            }

            constexpr split_outer_iterator & operator++()
            {
                auto & current = current_();
                const auto end = ranges::end(base_());
                if(current == end)
                    return *this;
                auto const pbegin = ranges::begin(parent_->pattern_);
                auto const pend = ranges::end(parent_->pattern_);
                if(pbegin == pend)
                    ++current;
                else
                    do
                    {
                        const auto ret = ranges::mismatch(current, end, pbegin, pend);
                        if(ret.in2 == pend)
                        {
                            current = ret.in1; // The pattern matched; skip it
                            break;
                        }
                    } while(++current != end);
                return *this;
            }

            constexpr decltype(auto) operator++(int)
            {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
                if constexpr(ForwardRange<Base>)
                {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
                else
                    ++*this;
#else
                return post_inc(meta::bool_<ForwardRange<Base>>{});
#endif
            }

            CPP_broken_friend_member
            friend constexpr auto operator==(split_outer_iterator const & x,
                                             split_outer_iterator const & y)
                -> CPP_broken_friend_ret(bool)( //
                    requires ForwardRange<Base>)
            {
                return x.curr_ == y.curr_;
            }
            CPP_broken_friend_member
            friend constexpr auto operator!=(split_outer_iterator const & x,
                                             split_outer_iterator const & y)
                -> CPP_broken_friend_ret(bool)( //
                    requires ForwardRange<Base>)
            {
                return x.curr_ != y.curr_;
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator==(split_outer_iterator const & x,
                                             default_sentinel_t)
            {
                return x.current_() == ranges::end(x.base_());
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator==(default_sentinel_t,
                                             split_outer_iterator const & x)
            {
                return x.current_() == ranges::end(x.base_());
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator!=(split_outer_iterator const & x,
                                             default_sentinel_t)
            {
                return x.current_() != ranges::end(x.base_());
            }
#ifdef RANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // RANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator!=(default_sentinel_t,
                                             split_outer_iterator const & x)
            {
                return x.current_() != ranges::end(x.base_());
            }
        };
    } // namespace detail
    /// \endcond

    template<typename V, typename Pattern>
#if CPP_CXX_CONCEPTS
        requires InputRange<V> && ForwardRange<Pattern> && View<V> && View<
            Pattern> && IndirectlyComparable<iterator_t<V>, iterator_t<Pattern>,
                                             ranges::equal_to> &&
        (ForwardRange<V> || detail::tiny_range<Pattern>)
#endif
            struct RANGES_EMPTY_BASES split_view
      : view_interface<split_view<V, Pattern>, is_finite<V>::value ? finite : unknown>
      , private detail::split_view_base<iterator_t<V>>
    {
    private:
        template<typename, bool>
        friend struct detail::split_outer_iterator;
        template<typename, bool>
        friend struct detail::split_inner_iterator;

        V base_ = V();
        Pattern pattern_ = Pattern();
        template<bool Const>
        using outer_iterator = detail::split_outer_iterator<split_view, Const>;

#if RANGES_CXX_IF_CONSTEXPR < RANGES_CXX_IF_CONSTEXPR_17
        outer_iterator<simple_view<V>()> begin_(std::true_type)
        {
            return outer_iterator<simple_view<V>()>{*this, ranges::begin(base_)};
        }
        outer_iterator<false> begin_(std::false_type)
        {
            this->curr_ = ranges::begin(base_);
            return outer_iterator<false>{*this};
        }

        outer_iterator<simple_view<V>()> end_(std::true_type) const
        {
            return outer_iterator<true>{*this, ranges::end(base_)};
        }
        default_sentinel_t end_(std::false_type) const
        {
            return default_sentinel;
        }
#endif

    public:
        split_view() = default;

        constexpr split_view(V base, Pattern pattern)
          : base_((V &&) base)
          , pattern_((Pattern &&) pattern)
        {}

        CPP_member
        constexpr CPP_ctor(split_view)(V base, range_value_t<V> e)(
            requires Constructible<Pattern, range_value_t<V>>)
          : base_(std::move(base))
          , pattern_(e)
        {}

        constexpr V base() const
        {
            return base_;
        }

        constexpr auto begin()
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr(ForwardRange<V>)
                return outer_iterator<simple_view<V>()>{*this, ranges::begin(base_)};
            else
            {
                this->curr_ = ranges::begin(base_);
                return outer_iterator<false>{*this};
            }
#else
            return begin_(meta::bool_<ForwardRange<V>>{});
#endif
        }
        CPP_member
        constexpr auto begin() const -> CPP_ret(outer_iterator<true>)( //
            requires ForwardRange<V> && ForwardRange<const V>)
        {
            return {*this, ranges::begin(base_)};
        }
        CPP_member
        constexpr auto end() -> CPP_ret(outer_iterator<simple_view<V>()>)( //
            requires ForwardRange<V> && CommonRange<V>)
        {
            return outer_iterator<simple_view<V>()>{*this, ranges::end(base_)};
        }
        constexpr auto end() const
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr(ForwardRange<V> && ForwardRange<const V> && CommonRange<const V>)
                return outer_iterator<true>{*this, ranges::end(base_)};
            else
                return default_sentinel;
#else
            return end_(meta::bool_ < ForwardRange<V> && ForwardRange<const V> &&
                        CommonRange<const V>> {});
#endif
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename R, typename P)( //
        requires InputRange<R> && ForwardRange<P> && ViewableRange<R> && ViewableRange<
            P> && IndirectlyComparable<iterator_t<R>, iterator_t<P>, ranges::equal_to> &&
        (ForwardRange<R> || detail::tiny_range<P>)) //
        split_view(R &&, P &&)
            ->split_view<view::all_t<R>, view::all_t<P>>;

    CPP_template(typename R)(   //
        requires InputRange<R>) //
        split_view(R &&, range_value_t<R>)
            ->split_view<view::all_t<R>, single_view<range_value_t<R>>>;
#endif

    namespace view
    {
        struct split_fn
        {
        private:
            friend view_access;

            template<typename T>
            static constexpr auto bind(split_fn split, T t)
            {
                return make_pipeable(bind_back(split, std::move(t)));
            }

        public:
            template<typename Rng>
            constexpr auto operator()(Rng && rng, range_value_t<Rng> val) const
                -> CPP_ret(split_view<all_t<Rng>, single_view<range_value_t<Rng>>>)( //
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        IndirectlyComparable<iterator_t<Rng>, range_value_t<Rng> const *,
                                             ranges::equal_to>)
            {
                return {all(static_cast<Rng &&>(rng)), single(std::move(val))};
            }

            template<typename Rng, typename Pattern>
            constexpr auto operator()(Rng && rng, Pattern && pattern) const
                -> CPP_ret(split_view<all_t<Rng>, all_t<Pattern>>)( //
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        ViewableRange<Pattern> && ForwardRange<Pattern> &&
                            IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>,
                                                 ranges::equal_to> &&
                    (ForwardRange<Rng> || detail::tiny_range<Pattern>))
            {
                return {all((Rng &&) rng), all((Pattern &&) pattern)};
            }
        };

        /// \relates split_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<split_fn>, split)
    } // namespace view

    namespace cpp20
    {
        namespace view
        {
            using ranges::view::split;
        }
        CPP_template(typename Rng, typename Pattern)( //
            requires InputRange<Rng> && ForwardRange<Pattern> && View<Rng> &&
                View<Pattern> && IndirectlyComparable<
                    iterator_t<Rng>, iterator_t<Pattern>, ranges::equal_to> &&
            (ForwardRange<Rng> || ranges::detail::tiny_range<Pattern>)) using split_view =
            ranges::split_view<Rng, Pattern>;
    } // namespace cpp20

    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::split_view)

#endif
