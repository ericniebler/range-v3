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
#include <range/v3/view/empty.hpp>

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
                auto operator()(std::intmax_t s, Rng &&rng)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    s * static_cast<std::intmax_t>(ranges::size(rng))
                )
            };

            template<typename... Views>
            using cartesian_product_cardinality = meta::fold<
                meta::list<range_cardinality<Views>...>,
                std::integral_constant<cardinality,
                    static_cast<cardinality>((sizeof...(Views) > 0))>,
                meta::quote<detail::product_cardinality>>;
        } // namespace detail

        template<typename... Views>
        class cartesian_product_view
          : public view_facade<cartesian_product_view<Views...>,
                detail::cartesian_product_cardinality<Views...>::value>
        {
            friend range_access;
            CONCEPT_ASSERT(sizeof...(Views) != 0);
            CONCEPT_ASSERT(meta::strict_and<ForwardView<Views>...>::value);
            using CanConst = meta::and_<Range<Views const>...>;
            template<bool IsConst>
            using CanSize = meta::and_<
                SizedRange<meta::if_c<IsConst, Views const, Views>>...>;
            template<bool IsConst>
            using CanDistance2_ = meta::and_<
                SizedSentinel<
                    iterator_t<meta::if_c<IsConst, Views const, Views>>,
                    iterator_t<meta::if_c<IsConst, Views const, Views>>>...>;
            template<bool IsConst>
            using CanDistance = meta::and_<
                CanSize<IsConst>,
                CanDistance2_<IsConst && CanSize<IsConst>::value>>;
            template<bool IsConst>
            using CanRandom2_ = meta::and_<
                RandomAccessIterator<iterator_t<
                    meta::if_c<IsConst, Views const, Views>>>...>;
            template<bool IsConst>
            using CanRandom = meta::and_<
                CanDistance<IsConst>,
                CanRandom2_<IsConst && CanDistance<IsConst>::value>>;
            template<bool IsConst>
            using CanBidi2_ = meta::and_<
                BidirectionalIterator<iterator_t<
                    meta::if_c<IsConst, Views const, Views>>>...>;
            template<bool IsConst>
            using Bounded = meta::and_<
                BoundedRange<meta::if_c<IsConst, Views const, Views>>...>;
            template<bool IsConst>
            using CanBidi = meta::or_<
                CanRandom<IsConst>,
                meta::and_<Bounded<IsConst>,
                    CanBidi2_<IsConst && Bounded<IsConst>::value>>>;

            static constexpr auto my_cardinality =
                detail::cartesian_product_cardinality<Views...>::value;

            std::tuple<Views...> views_;

            template<bool IsConst>
            class cursor
            {
                friend class cursor<!IsConst>;
                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;

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
                        CONCEPT_ASSERT(CanBidi<IsConst>());
                        // CanBidi<IsConst> implies this advance call is O(1)
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
                std::intmax_t distance_(cursor const &, meta::size_t<0>) const
                {
                    CONCEPT_ASSERT(sizeof...(Views) == 0);
                    return 0;
                }
                std::intmax_t distance_(cursor const &that, meta::size_t<1>) const
                {
                    return std::get<0>(that.its_) - std::get<0>(its_);
                }
                template<std::size_t N>
                std::intmax_t distance_(cursor const &that, meta::size_t<N>) const
                {
                    auto d = distance_(that, meta::size_t<N - 1>{});
                    d *= ranges::distance(std::get<N - 2>(view_->views_));
                    d += std::get<N - 1>(that.its_) - std::get<N - 1>(its_);
                    return d;
                }
                void advance_(meta::size_t<0>, std::intmax_t n)
                {
                    RANGES_EXPECT(sizeof...(Views) == 0);
                    RANGES_EXPECT(n == 0);
                }
RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO
                template<std::size_t N>
                void advance_(meta::size_t<N>, std::intmax_t n)
                {
                    if(n == 0)
                        return;

                    auto &i = std::get<N - 1>(its_);
                    auto const my_size = static_cast<std::intmax_t>(
                        ranges::size(std::get<N - 1>(view_->views_)));
                    auto const first = ranges::begin(std::get<N - 1>(view_->views_));

                    std::intmax_t const idx = i - first;
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

                    using D = difference_type_t<decltype(first)>;
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
                cursor(end_tag, constify_if<cartesian_product_view> &view, std::true_type) // Bounded
                  : cursor(begin_tag{}, view)
                {
                    CONCEPT_ASSERT(BoundedView<meta::at_c<meta::list<Views...>, 0>>());
                    std::get<0>(its_) = ranges::end(std::get<0>(view.views_));
                }
                cursor(end_tag, constify_if<cartesian_product_view> &view, std::false_type) // !Bounded
                  : cursor(begin_tag{}, view)
                {
                    using View0 = meta::at_c<meta::list<Views...>, 0>;
                    CONCEPT_ASSERT(!BoundedView<View0>() && RandomAccessRange<View0>() &&
                        SizedRange<View0>());
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
                  : cursor(end_tag{}, view, BoundedView<meta::at_c<meta::list<Views...>, 0>>{})
                {}
                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                cursor(cursor<Other> that)
                  : view_(that.view_)
                  , its_(std::move(that.its_))
                {}
                common_tuple<range_reference_t<Views>...> read() const
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
                CONCEPT_REQUIRES(CanBidi<IsConst>())
                void prev()
                {
                    prev_(meta::size_t<sizeof...(Views)>{});
                }
                CONCEPT_REQUIRES(CanDistance<IsConst>())
                std::intmax_t distance_to(cursor const &that) const
                {
                    return distance_(that, meta::size_t<sizeof...(Views)>{});
                }
                CONCEPT_REQUIRES(CanRandom<IsConst>())
                void advance(std::intmax_t n)
                {
                    advance_(meta::size_t<sizeof...(Views)>{}, n);
                }
            };
            cursor<false> begin_cursor()
            {
                return cursor<false>{begin_tag{}, *this};
            }
            CONCEPT_REQUIRES(CanConst())
            cursor<true> begin_cursor() const
            {
                return cursor<true>{begin_tag{}, *this};
            }
            CONCEPT_REQUIRES(CanBidi<false>())
            cursor<false> end_cursor()
            {
                return cursor<false>{end_tag{}, *this};
            }
            CONCEPT_REQUIRES(CanBidi<true>())
            cursor<true> end_cursor() const
            {
                return cursor<true>{end_tag{}, *this};
            }
            CONCEPT_REQUIRES(!CanBidi<true>())
            default_sentinel end_cursor() const
            {
                return {};
            }
        public:
            cartesian_product_view() = default;
            explicit constexpr cartesian_product_view(Views... views)
              : views_{detail::move(views)...}
            {}
            CONCEPT_REQUIRES(my_cardinality >= 0)
#ifdef RANGES_WORKAROUND_MSVC_DC338193
            constexpr std::intmax_t size() const noexcept
#else // ^^^ workaround / no workaround vvv
            constexpr static std::intmax_t size() noexcept
#endif // RANGES_WORKAROUND_MSVC_DC338193
            {
                return std::intmax_t{my_cardinality};
            }
            CONCEPT_REQUIRES(my_cardinality < 0 && CanSize<true>())
            std::intmax_t size() const
            {
                return tuple_foldl(views_, std::intmax_t{1},
                    detail::cartesian_size_fn{});
            }
            CONCEPT_REQUIRES(my_cardinality < 0 && CanSize<false>())
            std::intmax_t size()
            {
                return tuple_foldl(views_, std::intmax_t{1},
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
                    CONCEPT_REQUIRES_(sizeof...(Rngs) != 0 && Constraint<Rngs...>())>
                constexpr cartesian_product_view<all_t<Rngs>...>
                operator()(Rngs &&... rngs) const
                {
                    return cartesian_product_view<all_t<Rngs>...>{
                        all(static_cast<Rngs &&>(rngs))...};
                }

                CONCEPT_REQUIRES(Constraint<>())
                constexpr empty_view<std::tuple<>> operator()() const noexcept
                {
                    return {};
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
