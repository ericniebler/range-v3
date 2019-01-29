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

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/functional/indirect.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/empty.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct equal_to_
        {
            template<typename T, typename U>
            bool operator()(T const &t, U const &u) const
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
            template<typename I>
            auto operator()(I & i, iter_difference_t<I> n) const ->
                CPP_ret(void)(
                    requires Iterator<I>)
            {
                advance(i, n);
            }
        };
        RANGES_INLINE_VARIABLE(_advance_, advance_)

        struct distance_to_
        {
            template<typename T>
            constexpr auto operator()(T const &t, T const &u) const ->
                decltype(u - t)
            {
                return u - t;
            }
        };
        RANGES_INLINE_VARIABLE(distance_to_, distance_to)

        struct _min_
        {
            template<typename T, typename U>
            constexpr auto operator()(T const &t, U const &u) const ->
                decltype(true ? t : u)
            {
                return t < u ? t : u;
            }
        };
        RANGES_INLINE_VARIABLE(_min_, min_)

        struct _max_
        {
            template<typename T, typename U>
            constexpr auto operator()(T const &t, U const &u) const ->
                decltype(true ? u : t)
            {
                return t < u ? u : t;
            }
        };
        RANGES_INLINE_VARIABLE(_max_, max_)

        template<typename State, typename Value>
        using zip_cardinality =
            std::integral_constant<cardinality,
                State::value >= 0 || Value::value >= 0 ?
                    (State::value >= 0 && Value::value >= 0 ? min_(State::value, Value::value) : finite) :
                    State::value == finite || Value::value == finite ?
                        finite :
                        State::value == unknown || Value::value == unknown ?
                            unknown :
                            infinite>;
    } // namespace detail
    /// \endcond

    namespace view
    {
        CPP_def
        (
            template(typename Fun, typename ...Rngs)
            (concept IterZipWithViewConcept)(Fun, Rngs...),
                And<InputRange<Rngs>...> &&
                CopyConstructible<Fun> &&
                Invocable<Fun&, iterator_t<Rngs>...> &&
                Invocable<Fun&, copy_tag, iterator_t<Rngs>...> &&
                Invocable<Fun&, move_tag, iterator_t<Rngs>...>
        );
    }

    /// \addtogroup group-views
    /// @{
    template<typename Fun, typename...Rngs>
    struct iter_zip_with_view
      : view_facade<
            iter_zip_with_view<Fun, Rngs...>,
            meta::fold<
                meta::list<range_cardinality<Rngs>...>,
                std::integral_constant<cardinality, infinite>,
                meta::quote<detail::zip_cardinality>>::value>
    {
    private:
        CPP_assert(sizeof...(Rngs) != 0);
        friend range_access;

        semiregular_t<Fun> fun_;
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
            sentinel(detail::ignore_t, std::tuple<sentinel_t<meta::const_if_c<Const, Rngs>>...> ends)
              : ends_(std::move(ends))
            {}
            template<bool Other>
            CPP_ctor(sentinel)(sentinel<Other> that)(
                requires Const && (!Other))
              : ends_(std::move(that.ends_))
            {}
        };

        template<bool Const>
        struct cursor
        {
        private:
            friend struct cursor<!Const>;
            using fun_ref_ = semiregular_ref_or_val_t<Fun, Const>;
            fun_ref_ fun_;
            std::tuple<iterator_t<meta::const_if_c<Const, Rngs>>...> its_;

        public:
            using difference_type =
                common_type_t<range_difference_t<meta::const_if_c<Const, Rngs>>...>;
            using single_pass =
                meta::or_c<(bool) SinglePass<iterator_t<meta::const_if_c<Const, Rngs>>>...>;
            using value_type =
                detail::decay_t<invoke_result_t<fun_ref_ &, copy_tag, iterator_t<meta::const_if_c<Const, Rngs>>...>>;

            cursor() = default;
            cursor(fun_ref_ fun, std::tuple<iterator_t<meta::const_if_c<Const, Rngs>>...> its)
              : fun_(std::move(fun)), its_(std::move(its))
            {}
            template<bool Other>
            CPP_ctor(cursor)(cursor<Other> that)(
                requires Const && (!Other))
              : fun_(std::move(that.fun_)), its_(std::move(that.its_))
            {}
            auto CPP_auto_fun(read)() (const)
            (
                return tuple_apply(fun_, its_)
            )
            void next()
            {
                tuple_for_each(its_, detail::inc);
            }
            CPP_member
            auto equal(cursor const &that) const -> CPP_ret(bool)(
                requires And<Sentinel<
                    iterator_t<meta::const_if_c<Const, Rngs>>,
                    iterator_t<meta::const_if_c<Const, Rngs>>>...>)
            {
                // By returning true if *any* of the iterators are equal, we allow
                // zipped ranges to be of different lengths, stopping when the first
                // one reaches the end.
                return tuple_foldl(
                    tuple_transform(its_, that.its_, detail::equal_to),
                    false,
                    [](bool a, bool b) { return a || b; });
            }
            bool equal(sentinel<Const> const &s) const
            {
                // By returning true if *any* of the iterators are equal, we allow
                // zipped ranges to be of different lengths, stopping when the first
                // one reaches the end.
                return tuple_foldl(
                    tuple_transform(its_, s.ends_, detail::equal_to),
                    false,
                    [](bool a, bool b) { return a || b; });
            }
            CPP_member
            auto prev() -> CPP_ret(void)(
                requires And<BidirectionalRange<meta::const_if_c<Const, Rngs>>...>)
            {
                tuple_for_each(its_, detail::dec);
            }
            CPP_member
            auto advance(difference_type n) -> CPP_ret(void)(
                requires And<RandomAccessRange<meta::const_if_c<Const, Rngs>>...>)
            {
                using std::placeholders::_1;
                tuple_for_each(its_, std::bind(detail::advance_, _1, n));
            }
            CPP_member
            auto distance_to(cursor const &that) const ->
                CPP_ret(difference_type)(
                    requires And<SizedSentinel<
                        iterator_t<meta::const_if_c<Const, Rngs>>,
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
            template<std::size_t...Is>
            auto CPP_auto_fun(move_)(meta::index_sequence<Is...>) (const)
            (
                return invoke(fun_, move_tag{}, std::get<Is>(its_)...)
            )
            auto move() const
                noexcept(noexcept(std::declval<cursor const&>().move_(
                    meta::make_index_sequence<sizeof...(Rngs)>{}))) ->
                decltype(std::declval<cursor const&>().move_(
                    meta::make_index_sequence<sizeof...(Rngs)>{}))
            {
                return move_(meta::make_index_sequence<sizeof...(Rngs)>{});
            }
        };

        template<bool Const>
        using end_cursor_t =
            meta::if_c<
                concepts::and_v<(bool) CommonRange<Rngs>...,
                    !(bool) SinglePass<iterator_t<Rngs>>...>,
                cursor<Const>,
                sentinel<Const>>;

        cursor<false> begin_cursor()
        {
            return {fun_, tuple_transform(rngs_, begin)};
        }
        end_cursor_t<false> end_cursor()
        {
            return {fun_, tuple_transform(rngs_, end)};
        }
        template<bool Const = true>
        auto begin_cursor() const -> CPP_ret(cursor<Const>)(
            requires Const && And<Range<Rngs const>...> &&
                view::IterZipWithViewConcept<Fun, meta::if_c<Const, Rngs const>...>)
        {
            return {fun_, tuple_transform(rngs_, begin)};
        }
        template<bool Const = true>
        auto end_cursor() const -> CPP_ret(end_cursor_t<Const>)(
            requires Const && And<Range<Rngs const>...> &&
                view::IterZipWithViewConcept<Fun, meta::if_c<Const, Rngs const>...>)
        {
            return {fun_, tuple_transform(rngs_, end)};
        }
    public:
        iter_zip_with_view() = default;
        explicit iter_zip_with_view(Rngs ...rngs)
          : fun_(Fun{})
          , rngs_{std::move(rngs)...}
        {}
        explicit iter_zip_with_view(Fun fun, Rngs ...rngs)
          : fun_(std::move(fun))
          , rngs_{std::move(rngs)...}
        {}
        CPP_member
        constexpr auto CPP_fun(size)() (const
            requires And<SizedRange<Rngs const>...>)
        {
            using size_type = common_type_t<range_size_t<Rngs const>...>;
            return range_cardinality<iter_zip_with_view>::value >= 0 ?
                (size_type)range_cardinality<iter_zip_with_view>::value :
                tuple_foldl(
                    tuple_transform(
                        rngs_,
                        [](auto&& r) -> size_type { return ranges::size(r); }),
                    (std::numeric_limits<size_type>::max)(),
                    detail::min_);
        }
    };

    template<typename Fun, typename...Rngs>
    struct zip_with_view
      : iter_zip_with_view<indirected<Fun>, Rngs...>
    {
        CPP_assert(sizeof...(Rngs) != 0);

        zip_with_view() = default;
        explicit zip_with_view(Rngs ...rngs)
          : iter_zip_with_view<indirected<Fun>, Rngs...>{
              {Fun{}}, std::move(rngs)...}
        {}
        explicit zip_with_view(Fun fun, Rngs ...rngs)
          : iter_zip_with_view<indirected<Fun>, Rngs...>{
              {std::move(fun)}, std::move(rngs)...}
        {}
    };

    namespace view
    {
        struct iter_zip_with_fn
        {
            template<typename...Rngs, typename Fun>
            auto operator()(Fun fun, Rngs &&... rngs) const ->
                CPP_ret(iter_zip_with_view<Fun, all_t<Rngs>...>)(
                    requires And<ViewableRange<Rngs>...> &&
                        IterZipWithViewConcept<Fun, Rngs...> &&
                        sizeof...(Rngs) != 0)
            {
                return iter_zip_with_view<Fun, all_t<Rngs>...>{
                    std::move(fun),
                    all(static_cast<Rngs &&>(rngs))...
                };
            }

            template<typename Fun>
            constexpr auto operator()(Fun) const noexcept ->
                CPP_ret(empty_view<std::tuple<>>)(
                    requires IterZipWithViewConcept<Fun>)
            {
                return {};
            }
        };

        /// \relates iter_zip_with_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(iter_zip_with_fn, iter_zip_with)

        struct zip_with_fn
        {
            template<typename...Rngs, typename Fun>
            auto operator()(Fun fun, Rngs &&... rngs) const ->
                CPP_ret(zip_with_view<Fun, all_t<Rngs>...>)(
                    requires And<ViewableRange<Rngs>...> &&
                        And<InputRange<Rngs>...> &&
                        CopyConstructible<Fun> &&
                        Invocable<Fun&, range_reference_t<Rngs> &&...> &&
                        sizeof...(Rngs) != 0)
            {
                return zip_with_view<Fun, all_t<Rngs>...>{
                    std::move(fun),
                    all(static_cast<Rngs &&>(rngs))...
                };
            }

            template<typename Fun>
            constexpr auto operator()(Fun) const noexcept ->
                CPP_ret(empty_view<std::tuple<>>)(
                    requires CopyConstructible<Fun> && Invocable<Fun&>)
            {
                return {};
            }
        };

        /// \relates zip_with_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(zip_with_fn, zip_with)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::iter_zip_with_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::zip_with_view)

#endif
