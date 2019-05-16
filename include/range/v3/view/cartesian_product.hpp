/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014.
//  Copyright Casey Carter 2017.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_CARTESIAN_PRODUCT_HPP
#define RANGES_V3_VIEW_CARTESIAN_PRODUCT_HPP

#include <cstdint>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/view.hpp> // for dereference_fn

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename State, typename Value>
        using product_cardinality =
            std::integral_constant<cardinality,
                State::value == 0 || Value::value == 0 ?
                    static_cast<cardinality>(0) :
                    State::value == unknown || Value::value == unknown ?
                        unknown :
                        State::value == infinite || Value::value == infinite ?
                            infinite :
                            State::value == finite || Value::value == finite ?
                                finite :
                                static_cast<cardinality>(State::value * Value::value)>;

        struct cartesian_size_fn
        {
            template<typename Size, typename Rng>
            auto operator()(Size s, Rng &&rng) const ->
                CPP_ret(common_type_t<Size, range_size_t<Rng>>)(
                    requires IntegerLike_<Size> && SizedRange<Rng> &&
                        Common<Size, range_size_t<Rng>>)
            {
                using S = common_type_t<Size, range_size_t<Rng>>;
                return static_cast<S>(s) * static_cast<S>(ranges::size(rng));
            }
        };

        template<typename... Views>
        using cartesian_product_cardinality = meta::fold<
            meta::list<range_cardinality<Views>...>,
            std::integral_constant<cardinality,
                static_cast<cardinality>((sizeof...(Views) > 0))>,
            meta::quote<detail::product_cardinality>>;
    } // namespace detail
    /// \endcond

    CPP_def
    (
        template(typename...Views)
        (concept CartesianProductViewCanConst)(Views...),
            And<Range<Views const>...>
    );
    CPP_def
    (
        template(typename IsConst, typename...Views)
        (concept CartesianProductViewCanSize)(IsConst, Views...),
            And<SizedRange<meta::const_if<IsConst, Views>>...> &&
                Type<common_type_t<std::uintmax_t, range_size_t<meta::const_if<IsConst, Views>>...>>
    );
    CPP_def
    (
        template(typename IsConst, typename...Views)
        (concept CartesianProductViewCanDistance)(IsConst, Views...),
            CartesianProductViewCanSize<IsConst, Views...> &&
            And<SizedSentinel<
                iterator_t<meta::const_if<IsConst, Views>>,
                iterator_t<meta::const_if<IsConst, Views>>>...>
    );
    CPP_def
    (
        template(typename IsConst, typename...Views)
        (concept CartesianProductViewCanRandom)(IsConst, Views...),
            CartesianProductViewCanDistance<IsConst, Views...> &&
            And<RandomAccessIterator<iterator_t<
                meta::const_if<IsConst, Views>>>...>
    );
    CPP_def
    (
        template(typename IsConst, typename...Views)
        (concept CartesianProductViewCanBidi)(IsConst, Views...),
            CartesianProductViewCanRandom<IsConst, Views...> ||
            And<CommonRange<meta::const_if<IsConst, Views>>...,
                BidirectionalIterator<iterator_t<
                    meta::const_if<IsConst, Views>>>...>
    );

    template<typename... Views>
    class cartesian_product_view
      : public view_facade<cartesian_product_view<Views...>,
            detail::cartesian_product_cardinality<Views...>::value>
    {
    public: // BUGBUG
        friend range_access;
        CPP_assert(And<ForwardView<Views>...>);
        CPP_assert(sizeof...(Views) != 0);

        static constexpr auto my_cardinality =
            detail::cartesian_product_cardinality<Views...>::value;

        std::tuple<Views...> views_;

        template<bool IsConst_>
        class cursor
        {
            using IsConst = meta::bool_<IsConst_>;
            friend class cursor<!IsConst_>;
            template<typename T>
            using constify_if = meta::const_if_c<IsConst_, T>;
            constify_if<cartesian_product_view> *view_;
            std::tuple<iterator_t<constify_if<Views>>...> its_;

            void next_(meta::size_t<1>)
            {
                auto &v = std::get<0>(view_->views_);
                auto &i = std::get<0>(its_);
                auto const last = ranges::end(v);
                RANGES_EXPECT(i != last);
                ++i;
            }
            template<std::size_t N>
            void next_(meta::size_t<N>)
            {
                auto &v = std::get<N - 1>(view_->views_);
                auto &i = std::get<N - 1>(its_);
                auto const last = ranges::end(v);
                RANGES_EXPECT(i != last);
                if(++i == last)
                {
                    i = ranges::begin(v);
                    next_(meta::size_t<N - 1>{});
                }
            }
            void prev_(meta::size_t<0>)
            {
                RANGES_EXPECT(false);
            }
            template<std::size_t N>
            void prev_(meta::size_t<N>)
            {
                auto &v = std::get<N - 1>(view_->views_);
                auto &i = std::get<N - 1>(its_);
                if(i == ranges::begin(v))
                {
                    CPP_assert(CartesianProductViewCanBidi<IsConst, Views...>);
                    // CartesianProductViewCanBidi<IsConst, Views...> implies this advance call is O(1)
                    ranges::advance(i, ranges::end(v));
                    prev_(meta::size_t<N - 1>{});
                }
                --i;
            }
            bool equal_(cursor const &, meta::size_t<0>) const
            {
                return true;
            }
            template<std::size_t N>
            bool equal_(cursor const &that, meta::size_t<N>) const
            {
                return std::get<N - 1>(its_) == std::get<N - 1>(that.its_) &&
                    equal_(that, meta::size_t<N - 1>{});
            }
            auto distance_(cursor const &that, meta::size_t<1>) const
            {
                return (std::get<0>(that.its_) - std::get<0>(its_)) + std::intmax_t(0);
            }
            template<std::size_t N>
            auto distance_(cursor const &that, meta::size_t<N>) const
            {
                auto d = distance_(that, meta::size_t<N - 1>{});
                d *= ranges::distance(std::get<N - 2>(view_->views_));
                d += std::get<N - 1>(that.its_) - std::get<N - 1>(its_);
                return d;
            }
            template<typename Diff>
            void advance_(meta::size_t<0>, Diff)
            {
                RANGES_EXPECT(false);
            }
RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO
            template<std::size_t N, typename Diff>
            void advance_(meta::size_t<N>, Diff n)
            {
                if(n == 0)
                    return;

                auto &i = std::get<N - 1>(its_);
                auto const my_size = static_cast<Diff>(
                    ranges::size(std::get<N - 1>(view_->views_)));
                auto const first = ranges::begin(std::get<N - 1>(view_->views_));

                Diff const idx = static_cast<Diff>(i - first);
                RANGES_EXPECT(0 <= idx);
                RANGES_EXPECT(idx < my_size || (N == 1 && idx == my_size && n < 0));
                RANGES_EXPECT(n < INTMAX_MAX - idx);
                n += idx;

                auto n_div = n / my_size;
                auto n_mod = n % my_size;

                if RANGES_CONSTEXPR_IF(N != 1)
                {
                    if(n_mod < 0)
                    {
                        n_mod += my_size;
                        --n_div;
                    }
                    advance_(meta::size_t<N - 1>{}, n_div);
                }
                RANGES_EXPECT(0 <= n_mod && n_mod < my_size);

                if RANGES_CONSTEXPR_IF(N == 1)
                {
                    if(n_div > 0)
                    {
                        RANGES_EXPECT(n_div == 1);
                        RANGES_EXPECT(n_mod == 0);
                        n_mod = my_size;
                    }
                    else if(n_div < 0)
                    {
                        RANGES_EXPECT(n_div == -1);
                        RANGES_EXPECT(n_mod == 0);
                    }
                }

                using D = iter_difference_t<decltype(first)>;
                i = first + static_cast<D>(n_mod);
            }
RANGES_DIAGNOSTIC_POP
            void check_at_end_(meta::size_t<1>, bool at_end = false)
            {
                if(at_end)
                    ranges::advance(std::get<0>(its_), ranges::end(std::get<0>(view_->views_)));
            }
            template<std::size_t N>
            void check_at_end_(meta::size_t<N>, bool at_end = false)
            {
                return check_at_end_(
                    meta::size_t<N - 1>{},
                    at_end ||
                        bool(std::get<N - 1>(its_) == ranges::end(std::get<N - 1>(view_->views_))));
            }
            cursor(end_tag, constify_if<cartesian_product_view> &view, std::true_type) // Common
              : cursor(begin_tag{}, view)
            {
                CPP_assert(CommonView<meta::at_c<meta::list<Views...>, 0>>);
                std::get<0>(its_) = ranges::end(std::get<0>(view.views_));
            }
            cursor(end_tag, constify_if<cartesian_product_view> &view, std::false_type) // !Common
              : cursor(begin_tag{}, view)
            {
                using View0 = meta::at_c<meta::list<Views...>, 0>;
                CPP_assert(!CommonView<View0> && RandomAccessRange<View0> &&
                    SizedRange<View0>);
                std::get<0>(its_) += ranges::distance(std::get<0>(view.views_));
            }
        public:
            using value_type = std::tuple<range_value_t<Views>...>;

            cursor() = default;
            explicit cursor(begin_tag, constify_if<cartesian_product_view> &view)
              : view_(&view)
              , its_(tuple_transform(view.views_, ranges::begin))
            {
                // If any of the constituent views is empty, the cartesian_product is empty
                // and this "begin" iterator needs to become an "end" iterator.
                check_at_end_(meta::size_t<sizeof...(Views)>{});
            }
            explicit cursor(end_tag, constify_if<cartesian_product_view> &view)
              : cursor(end_tag{}, view,
                    meta::bool_<CommonView<meta::at_c<meta::list<Views...>, 0>>>{})
            {}
            CPP_template(bool Other)(
                requires IsConst_ && (!Other))
            cursor(cursor<Other> that)
              : view_(that.view_)
              , its_(std::move(that.its_))
            {}
            common_tuple<range_reference_t<Views>...> read() const
            {
                return tuple_transform(its_, detail::dereference_fn{});
            }
            void next()
            {
                next_(meta::size_t<sizeof...(Views)>{});
            }
            bool equal(default_sentinel_t) const
            {
                return std::get<0>(its_) == ranges::end(std::get<0>(view_->views_));
            }
            bool equal(cursor const &that) const
            {
                return equal_(that, meta::size_t<sizeof...(Views)>{});
            }
            CPP_member
            auto prev() -> CPP_ret(void)(
                requires CartesianProductViewCanBidi<IsConst, Views...>)
            {
                prev_(meta::size_t<sizeof...(Views)>{});
            }
            CPP_member
            auto CPP_fun(distance_to)(cursor const &that) (const
                requires CartesianProductViewCanDistance<IsConst, Views...>)
            {
                return distance_(that, meta::size_t<sizeof...(Views)>{});
            }
            template<typename Diff>
            auto advance(Diff n) -> CPP_ret(void)(
                requires CartesianProductViewCanRandom<IsConst, Views...> &&
                    detail::IntegerLike_<Diff>)
            {
                advance_(meta::size_t<sizeof...(Views)>{}, n);
            }
        };
        cursor<false> begin_cursor()
        {
            return cursor<false>{begin_tag{}, *this};
        }
        CPP_member
        auto begin_cursor() const -> CPP_ret(cursor<true>)(
            requires CartesianProductViewCanConst<Views...>)
        {
            return cursor<true>{begin_tag{}, *this};
        }
        CPP_member
        auto end_cursor() -> CPP_ret(cursor<false>)(
            requires CartesianProductViewCanBidi<std::false_type, Views...>)
        {
            return cursor<false>{end_tag{}, *this};
        }
        CPP_member
        auto end_cursor() const -> CPP_ret(cursor<true>)(
            requires CartesianProductViewCanBidi<std::true_type, Views...>)
        {
            return cursor<true>{end_tag{}, *this};
        }
        CPP_member
        auto end_cursor() const -> CPP_ret(default_sentinel_t)(
            requires (!CartesianProductViewCanBidi<std::true_type, Views...>))
        {
            return {};
        }
    public:
        cartesian_product_view() = default;
        explicit constexpr cartesian_product_view(Views... views)
          : views_{detail::move(views)...}
        {}
        CPP_template(int = 42)(
            requires (my_cardinality >= 0))
        static constexpr std::size_t size() noexcept
        {
            return std::size_t{my_cardinality};
        }
        CPP_member
        auto CPP_fun(size)() (const
            requires (my_cardinality < 0) &&
                CartesianProductViewCanSize<std::true_type, Views...>)
        {
            return tuple_foldl(views_, std::uintmax_t{1}, detail::cartesian_size_fn{});
        }
        CPP_member
        auto CPP_fun(size)() (
            requires (my_cardinality < 0) &&
                CartesianProductViewCanSize<std::false_type, Views...>)
        {
            return tuple_foldl(views_, std::uintmax_t{1}, detail::cartesian_size_fn{});
        }
    };

    namespace view
    {
        struct cartesian_product_fn
        {
            template<typename... Rngs>
            constexpr auto operator()(Rngs &&... rngs) const ->
                CPP_ret(cartesian_product_view<all_t<Rngs>...>)(
                    requires (sizeof...(Rngs) != 0) &&
                        concepts::And<(ForwardRange<Rngs> && ViewableRange<Rngs>)...>)
            {
                return cartesian_product_view<all_t<Rngs>...>{
                    all(static_cast<Rngs &&>(rngs))...};
            }

            constexpr empty_view<std::tuple<>> operator()() const noexcept
            {
                return {};
            }
        };

        RANGES_INLINE_VARIABLE(cartesian_product_fn, cartesian_product)
    }
} // namespace ranges

#endif
