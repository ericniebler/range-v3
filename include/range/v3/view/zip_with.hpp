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

#ifndef RANGES_V3_VIEW_ZIP_WITH_HPP
#define RANGES_V3_VIEW_ZIP_WITH_HPP

#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/indirect.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/facade.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct equal_to_
        {
            template<typename T, typename U>
            bool operator()(T const & t, U const & u) const
            {
                return static_cast<bool>(t == u);
            }
        };
        RANGES_INLINE_VARIABLE(equal_to_, equal_to)

        struct dec_
        {
            template<typename T>
            void operator()(T & t) const
            {
                --t;
            }
        };
        RANGES_INLINE_VARIABLE(dec_, dec)

        struct inc_
        {
            template<typename T>
            void operator()(T & t) const
            {
                ++t;
            }
        };
        RANGES_INLINE_VARIABLE(inc_, inc)

        struct _advance_
        {
            template<typename I, typename Diff>
            auto operator()(I & i, Diff n) const -> CPP_ret(void)( //
                requires input_or_output_iterator<I> && integer_like_<Diff>)
            {
                advance(i, static_cast<iter_difference_t<I>>(n));
            }
        };
        RANGES_INLINE_VARIABLE(_advance_, advance_)

        struct distance_to_
        {
            template<typename T>
            constexpr auto operator()(T const & t, T const & u) const -> decltype(u - t)
            {
                return u - t;
            }
        };
        RANGES_INLINE_VARIABLE(distance_to_, distance_to)

        struct _min_
        {
            template<typename T, typename U>
            constexpr auto operator()(T const & t, U const & u) const
                -> decltype(true ? t : u)
            {
                return u < t ? u : t;
            }
        };
        RANGES_INLINE_VARIABLE(_min_, min_)

        struct _max_
        {
            template<typename T, typename U>
            constexpr auto operator()(T const & t, U const & u) const
                -> decltype(true ? u : t)
            {
                return u < t ? t : u;
            }
        };
        RANGES_INLINE_VARIABLE(_max_, max_)

        template<typename State, typename Value>
        using zip_cardinality = std::integral_constant<
            cardinality, State::value >= 0 || Value::value >= 0
                             ? (State::value >= 0 && Value::value >= 0
                                    ? min_(State::value, Value::value)
                                    : finite)
                             : State::value == finite || Value::value == finite
                                   ? finite
                                   : State::value == unknown || Value::value == unknown
                                         ? unknown
                                         : infinite>;
    } // namespace detail
    /// \endcond

    namespace views
    {
        // clang-format off
        CPP_def
        (
            template(typename Fun, typename ...Rngs)
            (concept zippable_with)(Fun, Rngs...),
                and_v<input_range<Rngs>...> &&
                copy_constructible<Fun> &&
                invocable<Fun&, iterator_t<Rngs>...> &&
                invocable<Fun&, copy_tag, iterator_t<Rngs>...> &&
                invocable<Fun&, move_tag, iterator_t<Rngs>...>
        );
        // clang-format on
    } // namespace views

    /// \addtogroup group-views
    /// @{
    template<typename Fun, typename... Rngs>
    struct iter_zip_with_view
      : view_facade<iter_zip_with_view<Fun, Rngs...>,
                    meta::fold<meta::list<range_cardinality<Rngs>...>,
                               std::integral_constant<cardinality, infinite>,
                               meta::quote<detail::zip_cardinality>>::value>
    {
    private:
        CPP_assert(sizeof...(Rngs) != 0);
        friend range_access;

        semiregular_box_t<Fun> fun_;
        std::tuple<Rngs...> rngs_;
        using difference_type_ = common_type_t<range_difference_t<Rngs>...>;

        template<bool Const>
        struct cursor;

        template<bool Const>
        struct sentinel
        {
        private:
            friend struct cursor<Const>;
            friend struct sentinel<!Const>;
            std::tuple<sentinel_t<meta::const_if_c<Const, Rngs>>...> ends_;

        public:
            sentinel() = default;
            sentinel(detail::ignore_t,
                     std::tuple<sentinel_t<meta::const_if_c<Const, Rngs>>...> ends)
              : ends_(std::move(ends))
            {}
            CPP_template(bool Other)( //
                requires Const && (!Other)) sentinel(sentinel<Other> that)
              : ends_(std::move(that.ends_))
            {}
        };

        template<bool Const>
        struct cursor
        {
        private:
            friend struct cursor<!Const>;
            using fun_ref_ = semiregular_box_ref_or_val_t<Fun, Const>;
            fun_ref_ fun_;
            std::tuple<iterator_t<meta::const_if_c<Const, Rngs>>...> its_;

        public:
            using difference_type =
                common_type_t<range_difference_t<meta::const_if_c<Const, Rngs>>...>;
            using single_pass = meta::or_c<(
                bool)single_pass_iterator_<iterator_t<meta::const_if_c<Const, Rngs>>>...>;
            using value_type = detail::decay_t<invoke_result_t<
                fun_ref_ &, copy_tag, iterator_t<meta::const_if_c<Const, Rngs>>...>>;

            cursor() = default;
            cursor(fun_ref_ fun,
                   std::tuple<iterator_t<meta::const_if_c<Const, Rngs>>...> its)
              : fun_(std::move(fun))
              , its_(std::move(its))
            {}
            CPP_template(bool Other)( //
                requires Const && (!Other)) cursor(cursor<Other> that)
              : fun_(std::move(that.fun_))
              , its_(std::move(that.its_))
            {}
            // clang-format off
            auto CPP_auto_fun(read)()(const)
            (
                return tuple_apply(fun_, its_)
            )
                // clang-format on
                void next()
            {
                tuple_for_each(its_, detail::inc);
            }
            CPP_member
            auto equal(cursor const & that) const -> CPP_ret(bool)( //
                requires and_v<
                    sentinel_for<iterator_t<meta::const_if_c<Const, Rngs>>,
                                 iterator_t<meta::const_if_c<Const, Rngs>>>...>)
            {
                // By returning true if *any* of the iterators are equal, we allow
                // zipped ranges to be of different lengths, stopping when the first
                // one reaches the last.
                return tuple_foldl(tuple_transform(its_, that.its_, detail::equal_to),
                                   false,
                                   [](bool a, bool b) { return a || b; });
            }
            bool equal(sentinel<Const> const & s) const
            {
                // By returning true if *any* of the iterators are equal, we allow
                // zipped ranges to be of different lengths, stopping when the first
                // one reaches the last.
                return tuple_foldl(tuple_transform(its_, s.ends_, detail::equal_to),
                                   false,
                                   [](bool a, bool b) { return a || b; });
            }
            CPP_member
            auto prev() -> CPP_ret(void)( //
                requires and_v<bidirectional_range<meta::const_if_c<Const, Rngs>>...>)
            {
                tuple_for_each(its_, detail::dec);
            }
            CPP_member
            auto advance(difference_type n) -> CPP_ret(void)( //
                requires and_v<random_access_range<meta::const_if_c<Const, Rngs>>...>)
            {
                tuple_for_each(its_, bind_back(detail::advance_, n));
            }
            CPP_member
            auto distance_to(cursor const & that) const -> CPP_ret(difference_type)( //
                requires and_v<
                    sized_sentinel_for<iterator_t<meta::const_if_c<Const, Rngs>>,
                                       iterator_t<meta::const_if_c<Const, Rngs>>>...>)
            {
                // Return the smallest distance (in magnitude) of any of the iterator
                // pairs. This is to accommodate zippers of sequences of different length.
                if(0 < std::get<0>(that.its_) - std::get<0>(its_))
                    return tuple_foldl(
                        tuple_transform(its_, that.its_, detail::distance_to),
                        (std::numeric_limits<difference_type>::max)(),
                        detail::min_);
                else
                    return tuple_foldl(
                        tuple_transform(its_, that.its_, detail::distance_to),
                        (std::numeric_limits<difference_type>::min)(),
                        detail::max_);
            }
            // clang-format off
            template<std::size_t... Is>
            auto CPP_auto_fun(move_)(meta::index_sequence<Is...>)(const)
            (
                return invoke(fun_, move_tag{}, std::get<Is>(its_)...)
            )
                // clang-format on
                auto move() const noexcept(noexcept(std::declval<cursor const &>().move_(
                    meta::make_index_sequence<sizeof...(Rngs)>{})))
                    -> decltype(std::declval<cursor const &>().move_(
                        meta::make_index_sequence<sizeof...(Rngs)>{}))
            {
                return move_(meta::make_index_sequence<sizeof...(Rngs)>{});
            }
        };

        template<bool Const>
        using end_cursor_t =
            meta::if_c<concepts::and_v<(bool)common_range<Rngs>...,
                                       !(bool)single_pass_iterator_<iterator_t<Rngs>>...>,
                       cursor<Const>, sentinel<Const>>;

        cursor<false> begin_cursor()
        {
            return {fun_, tuple_transform(rngs_, ranges::begin)};
        }
        end_cursor_t<false> end_cursor()
        {
            return {fun_, tuple_transform(rngs_, ranges::end)};
        }
        template<bool Const = true>
        auto begin_cursor() const -> CPP_ret(cursor<Const>)( //
            requires Const && and_v<range<Rngs const>...> &&
                views::zippable_with<Fun, meta::if_c<Const, Rngs const>...>)
        {
            return {fun_, tuple_transform(rngs_, ranges::begin)};
        }
        template<bool Const = true>
        auto end_cursor() const -> CPP_ret(end_cursor_t<Const>)( //
            requires Const && and_v<range<Rngs const>...> &&
                views::zippable_with<Fun, meta::if_c<Const, Rngs const>...>)
        {
            return {fun_, tuple_transform(rngs_, ranges::end)};
        }

    public:
        iter_zip_with_view() = default;
        explicit iter_zip_with_view(Rngs... rngs)
          : fun_(Fun{})
          , rngs_{std::move(rngs)...}
        {}
        explicit iter_zip_with_view(Fun fun, Rngs... rngs)
          : fun_(std::move(fun))
          , rngs_{std::move(rngs)...}
        {}
        CPP_member
        constexpr auto CPP_fun(size)()(const requires and_v<sized_range<Rngs const>...>)
        {
            using size_type = common_type_t<range_size_t<Rngs const>...>;
            return range_cardinality<iter_zip_with_view>::value >= 0
                       ? size_type{(
                             std::size_t)range_cardinality<iter_zip_with_view>::value}
                       : tuple_foldl(tuple_transform(rngs_,
                                                     [](auto && r) -> size_type {
                                                         return ranges::size(r);
                                                     }),
                                     (std::numeric_limits<size_type>::max)(),
                                     detail::min_);
        }
    };

    template<typename Fun, typename... Rngs>
    struct zip_with_view : iter_zip_with_view<indirected<Fun>, Rngs...>
    {
        CPP_assert(sizeof...(Rngs) != 0);

        zip_with_view() = default;
        explicit zip_with_view(Rngs... rngs)
          : iter_zip_with_view<indirected<Fun>, Rngs...>{{Fun{}}, std::move(rngs)...}
        {}
        explicit zip_with_view(Fun fun, Rngs... rngs)
          : iter_zip_with_view<indirected<Fun>, Rngs...>{{std::move(fun)},
                                                         std::move(rngs)...}
        {}
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Fun, typename... Rng)(requires copy_constructible<Fun>)
        zip_with_view(Fun, Rng &&...)
            ->zip_with_view<Fun, views::all_t<Rng>...>;
#endif

    namespace views
    {
        struct iter_zip_with_fn
        {
            template<typename... Rngs, typename Fun>
            auto operator()(Fun fun, Rngs &&... rngs) const -> CPP_ret(
                iter_zip_with_view<Fun, all_t<Rngs>...>)( //
                requires and_v<viewable_range<Rngs>...> && zippable_with<Fun, Rngs...> &&
                (sizeof...(Rngs) != 0))
            {
                return iter_zip_with_view<Fun, all_t<Rngs>...>{
                    std::move(fun), all(static_cast<Rngs &&>(rngs))...};
            }

            template<typename Fun>
            constexpr auto operator()(Fun) const noexcept
                -> CPP_ret(empty_view<std::tuple<>>)( //
                    requires zippable_with<Fun>)
            {
                return {};
            }
        };

        /// \relates iter_zip_with_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(iter_zip_with_fn, iter_zip_with)

        struct zip_with_fn
        {
            template<typename... Rngs, typename Fun>
            auto operator()(Fun fun, Rngs &&... rngs) const
                -> CPP_ret(zip_with_view<Fun, all_t<Rngs>...>)( //
                    requires and_v<viewable_range<Rngs>...> &&
                        and_v<input_range<Rngs>...> && copy_constructible<Fun> &&
                            invocable<Fun &, range_reference_t<Rngs> &&...> &&
                    (sizeof...(Rngs) != 0))
            {
                return zip_with_view<Fun, all_t<Rngs>...>{
                    std::move(fun), all(static_cast<Rngs &&>(rngs))...};
            }

            template<typename Fun>
            constexpr auto operator()(Fun) const noexcept
                -> CPP_ret(empty_view<std::tuple<>>)( //
                    requires copy_constructible<Fun> && invocable<Fun &>)
            {
                return {};
            }
        };

        /// \relates zip_with_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(zip_with_fn, zip_with)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::iter_zip_with_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::zip_with_view)

#endif
