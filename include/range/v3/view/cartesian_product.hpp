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
                template<typename Rng, CONCEPT_REQUIRES_(SizedRange<Rng>())>
                auto operator()(std::size_t s, Rng &&rng)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    s * static_cast<std::size_t>(ranges::size(rng))
                )
            };
        } // namespace detail

        template<typename...Views>
        class cartesian_product_view
          : public view_facade<cartesian_product_view<Views...>,
                meta::fold<
                    meta::list<range_cardinality<Views>...>,
                    std::integral_constant<cardinality,
                        static_cast<cardinality>((sizeof...(Views) > 0))>,
                    meta::quote<detail::product_cardinality>>::value>
        {
            friend range_access;
            CONCEPT_ASSERT(meta::strict_and<ForwardView<Views>...>::value);
            using CanConst = meta::strict_and<
                Range<Views const>...>;
            template<bool IsConst>
            using CanSize = meta::strict_and<
                SizedRange<meta::if_c<IsConst, Views const, Views>>...>;
            template<bool IsConst>
            using CanDistance = meta::strict_and<
                CanSize<IsConst>,
                SizedSentinel<
                    iterator_t<meta::if_c<IsConst, Views const, Views>>,
                    iterator_t<meta::if_c<IsConst, Views const, Views>>>...>;
            template<bool IsConst>
            using CanRandom = meta::strict_and<
                CanDistance<IsConst>,
                RandomAccessIterator<iterator_t<
                    meta::if_c<IsConst, Views const, Views>>>...>;
            template<bool IsConst>
            using CanBidi = meta::strict_or<
                CanRandom<IsConst>,
                meta::strict_and<
                    BoundedRange<meta::if_c<IsConst, Views const, Views>>...,
                    BidirectionalIterator<iterator_t<
                        meta::if_c<IsConst, Views const, Views>>>...>>;

            std::tuple<Views...> views_;

            template<bool IsConst>
            class cursor
            {
                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;
                using pos_t = std::tuple<iterator_t<constify_if<Views>>...>;
                constify_if<cartesian_product_view> *view_;
                pos_t its_;

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
                        CONCEPT_ASSERT(CanBidi<IsConst>());
                        // CanBidi<IsConst> implies this advance call is O(1)
                        ranges::advance(i, ranges::end(v));
                        prev_(meta::size_t<N - 1>{});
                    }
                    --i;
                }
                bool equal_(cursor const &, meta::size_t<sizeof...(Views)>) const
                {
                    return true;
                }
                template<std::size_t N>
                bool equal_(cursor const &that, meta::size_t<N>) const
                {
                    return std::get<N>(its_) == std::get<N>(that.its_) &&
                        equal_(that, meta::size_t<N + 1>{});
                }
                struct dist_info {
                    std::ptrdiff_t distance;
                    std::ptrdiff_t size_product;

                    dist_info(std::ptrdiff_t d = 0, std::ptrdiff_t s = 1)
                      : distance{d}, size_product{s}
                    {}
                };
                std::ptrdiff_t distance_(
                    cursor const &, meta::size_t<0>, dist_info) const
                {
                    CONCEPT_ASSERT(sizeof...(Views) == 0);
                    return 0;
                }
                std::ptrdiff_t distance_(
                    cursor const &that, meta::size_t<1>, dist_info const inf) const
                {
                    auto const my_distance = std::get<0>(that.its_) - std::get<0>(its_);
                    return my_distance * inf.size_product + inf.distance;
                }
                template<std::size_t N>
                std::ptrdiff_t distance_(
                    cursor const &that, meta::size_t<N>, dist_info const inf) const
                {
                    auto const my_size = ranges::size(std::get<N - 1>(view_->views_));
                    auto const my_distance = std::get<N - 1>(that.its_) - std::get<N - 1>(its_);
                    return distance_(that, meta::size_t<N - 1>{}, dist_info{
                        static_cast<std::ptrdiff_t>(my_distance * inf.size_product + inf.distance),
                        static_cast<std::ptrdiff_t>(my_size) * inf.size_product
                    });
                }
                void advance_(meta::size_t<0>, dist_info const inf)
                {
                    RANGES_EXPECT(inf.distance == 0);
                }
                template<std::size_t N>
                void advance_(meta::size_t<N>, dist_info const inf)
                {
                    auto &i = std::get<N - 1>(its_);
                    auto const my_size = ranges::size(std::get<N - 1>(view_->views_));
                    auto const first = ranges::begin(std::get<N - 1>(view_->views_));
                    auto const idx = i - first;
                    auto d = inf.distance;
                    if(static_cast<std::ptrdiff_t>(my_size) - idx < d || d < -idx)
                    {
                        auto const new_size = inf.size_product * static_cast<std::ptrdiff_t>(my_size);
                        auto div = d / new_size;
                        d %= new_size;
                        if(static_cast<std::ptrdiff_t>(my_size) - idx < d)
                        {
                            i = first;
                            d -= static_cast<std::ptrdiff_t>(my_size) - idx;
                            ++div;
                            RANGES_EXPECT(0 <= d && d < static_cast<std::ptrdiff_t>(my_size));
                        }
                        else if(d < -idx)
                        {
                            i = first + static_cast<std::ptrdiff_t>(my_size);
                            d += idx;
                            --div;
                            RANGES_EXPECT(0 > d && -d <= static_cast<std::ptrdiff_t>(my_size));
                        }
                        advance_(meta::size_t<N - 1>{}, { div, new_size });
                    }
                    i += d;
                }
                void check_at_end_(meta::size_t<0>, bool = false)
                {}
                void check_at_end_(meta::size_t<1>, bool at_end = false)
                {
                    if(at_end)
                    {
                        ranges::advance(std::get<0>(its_), ranges::end(std::get<0>(view_->views_)));
                    }
                }
                template<std::size_t N>
                void check_at_end_(meta::size_t<N>, bool at_end = false)
                {
                    return check_at_end_(meta::size_t<N - 1>{}, at_end ||
                        std::get<N - 1>(its_) == ranges::end(std::get<N - 1>(view_->views_)));
                }
            public:
                using value_type = std::tuple<range_value_type_t<Views>...>;
                cursor() = default;
                explicit cursor(begin_tag, constify_if<cartesian_product_view> &view)
                  : view_(&view)
                  , its_(tuple_transform(view.views_, ranges::begin))
                {
                    check_at_end_(meta::size_t<sizeof...(Views)>{});
                }
                explicit cursor(end_tag, constify_if<cartesian_product_view> &view)
                  : cursor(begin_tag{}, view)
                {
                    std::get<0>(its_) = ranges::end(std::get<0>(view.views_));
                }
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
                    return equal_(that, meta::size_t<0>{});
                }
                CONCEPT_REQUIRES(CanBidi<IsConst>())
                void prev()
                {
                    prev_(meta::size_t<sizeof...(Views)>{});
                }
                CONCEPT_REQUIRES(CanDistance<IsConst>())
                std::ptrdiff_t distance_to(cursor const &that) const
                {
                    return distance_(that, meta::size_t<sizeof...(Views)>{}, {});
                }
                CONCEPT_REQUIRES(CanRandom<IsConst>())
                void advance(std::ptrdiff_t n)
                {
                    return advance_(meta::size_t<sizeof...(Views)>{}, { n });
                }
            };
            CONCEPT_REQUIRES(CanConst())
            cursor<true> begin_cursor() const
            {
                return cursor<true>{begin_tag{}, *this};
            }
            CONCEPT_REQUIRES(!CanConst())
            cursor<false> begin_cursor()
            {
                return cursor<false>{begin_tag{}, *this};
            }
            CONCEPT_REQUIRES(sizeof...(Views) == 0)
            cursor<true> end_cursor() const
            {
                return cursor<true>{begin_tag{}, *this};
            }
            CONCEPT_REQUIRES(CanBidi<true>() && sizeof...(Views) > 0)
            cursor<true> end_cursor() const
            {
                return cursor<true>{end_tag{}, *this};
            }
            CONCEPT_REQUIRES(CanBidi<false>() && !CanBidi<true>())
            cursor<false> end_cursor()
            {
                return cursor<false>{end_tag{}, *this};
            }
            CONCEPT_REQUIRES(!CanBidi<true>())
            default_sentinel end_cursor() const
            {
                return {};
            }
        public:
            cartesian_product_view() = default;
            CONCEPT_REQUIRES(sizeof...(Views) > 0)
            constexpr cartesian_product_view(Views... views)
              : views_{detail::move(views)...}
            {}
            CONCEPT_REQUIRES(CanSize<true>())
            std::size_t size() const
            {
                if(sizeof...(Views) == 0) return 0;
                return tuple_foldl(views_, std::size_t{1},
                    detail::cartesian_size_fn{});
            }
            CONCEPT_REQUIRES(CanSize<false>() && !CanSize<true>())
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
                template<typename... Rngs>
                using Constraint = meta::strict_and<ForwardRange<Rngs>...>;

                template<typename... Rngs,
                    CONCEPT_REQUIRES_(Constraint<Rngs...>())>
                constexpr cartesian_product_view<all_t<Rngs>...> operator()(Rngs &&... rngs) const
                {
                    return cartesian_product_view<all_t<Rngs>...>{all((Rngs &&) rngs)...};
                }

                template<typename... Rngs,
                    CONCEPT_REQUIRES_(!Constraint<Rngs...>())>
                void operator()(Rngs &&...) const
                {
                    static_assert(meta::strict_and<ForwardRange<Rngs>...>(),
                        "All of the ranges passed to view::cartesian_product must model the "
                        "ForwardRange concept.");
                }
            };

            RANGES_INLINE_VARIABLE(cartesian_product_fn, cartesian_product)
        }
    } // namespace v3
} // namespace ranges

#endif
