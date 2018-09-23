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

#include <range/v3/begin_end.hpp>
#include <range/v3/range_access.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
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
                CPP_template(typename Rng)(
                    requires SizedRange<Rng>)
                auto operator()(std::size_t s, Rng &&rng)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    s * static_cast<std::size_t>(ranges::size(rng))
                )
            };
        } // namespace detail

        CPP_def(
            template(typename...Views)
            (concept CartesianProductViewCanConst)(Views...),
                And<Range<Views const>...>
        );
        CPP_def(
            template(typename IsConst, typename...Views)
            (concept CartesianProductViewCanSize)(IsConst, Views...),
                And<SizedRange<meta::const_if<IsConst, Views>>...>
        );
        CPP_def(
            template(typename IsConst, typename...Views)
            (concept CartesianProductViewCanDistance)(IsConst, Views...),
                CartesianProductViewCanSize<IsConst, Views...> &&
                And<SizedSentinel<
                    iterator_t<meta::const_if<IsConst, Views>>,
                    iterator_t<meta::const_if<IsConst, Views>>>...>
        );
        CPP_def(
            template(typename IsConst, typename...Views)
            (concept CartesianProductViewCanRandom)(IsConst, Views...),
                CartesianProductViewCanDistance<IsConst, Views...> &&
                And<RandomAccessIterator<iterator_t<
                    meta::const_if<IsConst, Views>>>...>
        );
        CPP_def(
            template(typename IsConst, typename...Views)
            (concept CartesianProductViewCanBidi)(IsConst, Views...),
                // BUGBUG alternation is totally broken.
                True<> &&
                (CartesianProductViewCanRandom<IsConst, Views...> ||
                And<BoundedRange<meta::const_if<IsConst, Views>>...,
                    BidirectionalIterator<iterator_t<
                        meta::const_if<IsConst, Views>>>...>)
        );

        template<typename...Views>
        class cartesian_product_view
          : public view_facade<cartesian_product_view<Views...>,
                meta::fold<
                    meta::list<range_cardinality<Views>...>,
                    std::integral_constant<cardinality,
                        static_cast<cardinality>((sizeof...(Views) > 0))>,
                    meta::quote<detail::product_cardinality>>::value>
        {
        public: // BUGBUG
            friend range_access;
            CPP_assert(And<ForwardView<Views>...>);

            std::tuple<Views...> views_;

            template<bool IsConst_>
            class cursor
            {
                using IsConst = meta::bool_<IsConst_>;
                template<typename T>
                using constify_if = meta::const_if_c<IsConst_, T>;
                constify_if<cartesian_product_view> *view_;
                std::tuple<iterator_t<constify_if<Views>>...> its_;

                void next_(meta::size_t<0>)
                {
                    RANGES_EXPECT(false);
                }
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
                std::ptrdiff_t distance_(cursor const &, meta::size_t<0>) const
                {
                    CPP_assert(sizeof...(Views) == 0);
                    return 0;
                }
                std::ptrdiff_t distance_(cursor const &that, meta::size_t<1>) const
                {
                    return static_cast<std::ptrdiff_t>(
                        std::get<0>(that.its_) - std::get<0>(its_));
                }
                template<std::size_t N>
                std::ptrdiff_t distance_(cursor const &that, meta::size_t<N>) const
                {
                    auto d = distance_(that, meta::size_t<N - 1>{});
                    d *= static_cast<std::ptrdiff_t>(
                        ranges::distance(std::get<N - 2>(view_->views_)));
                    d += static_cast<std::ptrdiff_t>(
                        std::get<N - 1>(that.its_) - std::get<N - 1>(its_));
                    return d;
                }
                void advance_(meta::size_t<0>, std::ptrdiff_t n)
                {
                    RANGES_EXPECT(n == 0);
                }
                template<std::size_t N>
                void advance_(meta::size_t<N>, std::ptrdiff_t n)
                {
                    if(n == 0) return;

                    auto &i = std::get<N - 1>(its_);
                    auto const my_size = static_cast<std::ptrdiff_t>(
                        ranges::size(std::get<N - 1>(view_->views_)));
                    auto const first = ranges::begin(std::get<N - 1>(view_->views_));

                    auto const idx = i - first;
                    RANGES_EXPECT(0 <= idx && idx < my_size);
                    RANGES_EXPECT(n < PTRDIFF_MAX - idx);
                    n += idx;

                    if RANGES_CONSTEXPR_IF(N != 1)
                    {
                        auto const borrow = n < 0;
                        advance_(meta::size_t<N - 1>{}, n / my_size - borrow);
                        n %= my_size;
                        if(borrow)
                            n += my_size;
                    }
                    RANGES_EXPECT(0 <= n);
                    RANGES_EXPECT(n < my_size || (N == 1 && n == my_size));
                    i = first + n;
                }
                void check_at_end_(meta::size_t<0>, bool = false)
                {}
                void check_at_end_(meta::size_t<1>, bool at_end = false)
                {
                    if(at_end)
                        ranges::advance(std::get<0>(its_), ranges::end(std::get<0>(view_->views_)));
                }
                template<std::size_t N>
                void check_at_end_(meta::size_t<N>, bool at_end = false)
                {
                    if(!at_end)
                        at_end = std::get<N - 1>(its_) == ranges::end(std::get<N - 1>(view_->views_));
                    return check_at_end_(meta::size_t<N - 1>{}, at_end);
                }
                cursor(end_tag, constify_if<cartesian_product_view> &view, std::true_type) // Bounded
                  : cursor(begin_tag{}, view)
                {
                    CPP_assert(BoundedView<meta::at_c<meta::list<Views...>, 0>>);
                    std::get<0>(its_) = ranges::end(std::get<0>(view.views_));
                }
                cursor(end_tag, constify_if<cartesian_product_view> &view, std::false_type) // !Bounded
                  : cursor(begin_tag{}, view)
                {
                    using View0 = meta::at_c<meta::list<Views...>, 0>;
                    CPP_assert(!BoundedView<View0> && RandomAccessRange<View0> &&
                        SizedRange<View0>);
                    std::get<0>(its_) += ranges::distance(std::get<0>(view.views_));
                }
            public:
                using value_type = std::tuple<range_value_type_t<Views>...>;

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
                        meta::bool_<BoundedView<meta::at_c<meta::list<Views...>, 0>>>{})
                {}
                ranges::common_tuple<range_reference_t<Views>...> read() const
                {
                    return tuple_transform(its_, ranges::dereference);
                }
                void next()
                {
                    next_(meta::size_t<sizeof...(Views)>{});
                }
                bool equal(default_sentinel) const
                {
                    return std::get<0>(its_) == ranges::end(std::get<0>(view_->views_));
                }
                bool equal(cursor const &that) const
                {
                    return equal_(that, meta::size_t<sizeof...(Views)>{});
                }
                CPP_requires(CartesianProductViewCanBidi<IsConst, Views...>)
                void prev()
                {
                    prev_(meta::size_t<sizeof...(Views)>{});
                }
                CPP_requires(CartesianProductViewCanDistance<IsConst, Views...>)
                std::ptrdiff_t distance_to(cursor const &that) const
                {
                    return distance_(that, meta::size_t<sizeof...(Views)>{});
                }
                CPP_requires(CartesianProductViewCanRandom<IsConst, Views...>)
                void advance(std::ptrdiff_t n)
                {
                    advance_(meta::size_t<sizeof...(Views)>{}, n);
                }
            };
            CPP_requires(CartesianProductViewCanConst<Views...>)
            cursor<true> begin_cursor() const
            {
                return cursor<true>{begin_tag{}, *this};
            }
            CPP_requires(not CartesianProductViewCanConst<Views...>)
            cursor<false> begin_cursor()
            {
                return cursor<false>{begin_tag{}, *this};
            }
            CPP_requires(sizeof...(Views) == 0)
            cursor<true> end_cursor() const
            {
                return cursor<true>{begin_tag{}, *this};
            }
            CPP_requires(CartesianProductViewCanBidi<std::true_type, Views...> && (sizeof...(Views) > 0))
            cursor<true> end_cursor() const
            {
                return cursor<true>{end_tag{}, *this};
            }
            CPP_requires(CartesianProductViewCanBidi<std::false_type, Views...> &&
                !CartesianProductViewCanBidi<std::true_type, Views...>)
            cursor<false> end_cursor()
            {
                return cursor<false>{end_tag{}, *this};
            }
            CPP_requires(not CartesianProductViewCanBidi<std::true_type, Views...>)
            default_sentinel end_cursor() const
            {
                return {};
            }
        public:
            cartesian_product_view() = default;
            CPP_requires(sizeof...(Views) > 0)
            constexpr cartesian_product_view(Views... views)
              : views_{detail::move(views)...}
            {}
            CPP_requires(CartesianProductViewCanSize<std::true_type, Views...>)
            std::size_t size() const
            {
                if(sizeof...(Views) == 0) return 0;
                return tuple_foldl(views_, std::size_t{1},
                    detail::cartesian_size_fn{});
            }
            CPP_requires(CartesianProductViewCanSize<std::false_type, Views...> &&
                !CartesianProductViewCanSize<std::true_type, Views...>)
            std::size_t size()
            {
                return tuple_foldl(views_, std::size_t{1},
                    detail::cartesian_size_fn{});
            }
        };

        namespace view
        {
            struct cartesian_product_fn
            {
                CPP_template(typename... Rngs)(
                    requires concepts::And<ForwardRange<Rngs>...>)
                constexpr cartesian_product_view<all_t<Rngs>...> operator()(Rngs &&... rngs) const
                {
                    return cartesian_product_view<all_t<Rngs>...>{all((Rngs &&) rngs)...};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename... Rngs)(
                    requires not concepts::And<ForwardRange<Rngs>...>)
                void operator()(Rngs &&...) const
                {
                    static_assert(And<ForwardRange<Rngs>...>,
                        "All of the ranges passed to view::cartesian_product must model the "
                        "ForwardRange concept.");
                }
            #endif
            };

            RANGES_INLINE_VARIABLE(cartesian_product_fn, cartesian_product)
        }
    } // namespace v3
} // namespace ranges

#endif
