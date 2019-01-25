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

#ifndef RANGES_V3_ACTION_INSERT_HPP
#define RANGES_V3_ACTION_INSERT_HPP

#include <functional>
#include <initializer_list>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/iterator/common_iterator.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace adl_insert_detail
    {
        template<typename Cont, typename ...Args>
        using insert_result_t =
            decltype(unwrap_reference(std::declval<Cont>())
                .insert(std::declval<Args>()...));

        template<typename Cont, typename T>
        auto insert(Cont &&cont, T &&t) -> CPP_ret(
            insert_result_t<Cont&, T>)(
            requires LvalueContainerLike<Cont> &&
                (!Range<T> && Constructible<range_value_t<Cont>, T>))
        {
            return unwrap_reference(cont).insert(static_cast<T &&>(t));
        }

        template<typename Cont, typename I, typename S>
        auto insert(Cont &&cont, I i, S j) -> CPP_ret(
            insert_result_t<
                Cont&,
                common_iterator_t<I, S>,
                common_iterator_t<I, S>>)(
            requires LvalueContainerLike<Cont> && Sentinel<S, I> && !Range<S>)
        {
            return unwrap_reference(cont).insert(
                common_iterator_t<I, S>{i},
                common_iterator_t<I, S>{j});
        }

        template<typename Cont, typename Rng>
        auto insert(Cont &&cont, Rng &&rng) -> CPP_ret(
            insert_result_t<
                Cont&,
                range_common_iterator_t<Rng>,
                range_common_iterator_t<Rng>>)(
            requires LvalueContainerLike<Cont> && Range<Rng>)
        {
            return unwrap_reference(cont).insert(
                range_common_iterator_t<Rng>{ranges::begin(rng)},
                range_common_iterator_t<Rng>{ranges::end(rng)});
        }

        template<typename Cont, typename I, typename T>
        auto insert(Cont &&cont, I p, T &&t) -> CPP_ret(
            insert_result_t<Cont&, I, T>)(
            requires LvalueContainerLike<Cont> && Iterator<I> &&
                (!Range<T> && Constructible<range_value_t<Cont>, T>))
        {
            return unwrap_reference(cont).insert(p, static_cast<T &&>(t));
        }

        template<typename Cont, typename I, typename N, typename T>
        auto insert(Cont &&cont, I p, N n, T &&t) -> CPP_ret(
            insert_result_t<Cont&, I, N, T>)(
            requires LvalueContainerLike<Cont> && Iterator<I> &&
                Integral<N> && Constructible<range_value_t<Cont>, T>)
        {
            return unwrap_reference(cont).insert(p, n, static_cast<T &&>(t));
        }

        /// \cond
        namespace detail
        {
            template<typename Cont, typename P>
            auto insert_reserve_helper(
                Cont &cont, P const p, range_size_t<Cont> const delta) ->
                CPP_ret(iterator_t<Cont>)(
                requires Container<Cont> && Iterator<P> &&
                    RandomAccessReservable<Cont>)
            {
                auto const old_size = ranges::size(cont);
                auto const max_size = cont.max_size();
                RANGES_EXPECT(delta <= max_size - old_size);
                auto const new_size = old_size + delta;
                auto const old_capacity = cont.capacity();
                auto const index = p - ranges::begin(cont);
                if (old_capacity < new_size)
                {
                    auto const new_capacity = (old_capacity <= max_size / 3 * 2)
                        ? ranges::max(old_capacity + old_capacity / 2, new_size)
                        : max_size;
                    cont.reserve(new_capacity);
                }
                return ranges::begin(cont) + index;
            }

            template<typename Cont, typename P, typename I, typename S>
            auto insert_impl(Cont &&cont, P p, I i, S j, std::false_type) ->
                CPP_ret(decltype(unwrap_reference(cont).
                    insert(p, common_iterator_t<I, S>{i}, common_iterator_t<I, S>{j})))(
                requires Sentinel<S, I> && !Range<S>)
            {
                using C = common_iterator_t<I, S>;
                return unwrap_reference(cont).insert(p, C{i}, C{j});
            }

            template<typename Cont, typename P, typename I, typename S>
            auto insert_impl(Cont &&cont_, P p, I i, S j, std::true_type) ->
                CPP_ret(decltype(unwrap_reference(cont_).insert(
                    ranges::begin(unwrap_reference(cont_)),
                    common_iterator_t<I, S>{i},
                    common_iterator_t<I, S>{j})))(
                requires SizedSentinel<S, I> && RandomAccessReservable<Cont> && !Range<S>)
            {
                using C = common_iterator_t<I, S>;
                auto &&cont = unwrap_reference(cont_);
                auto const delta = static_cast<range_size_t<Cont>>(j - i);
                auto pos = insert_reserve_helper(cont, std::move(p), delta);
                return cont.insert(pos, C{std::move(i)}, C{std::move(j)});
            }

            template<typename Cont, typename I, typename Rng>
            auto insert_impl(Cont &&cont, I p, Rng &&rng, std::false_type) ->
                CPP_ret(decltype(unwrap_reference(cont).insert(
                    p,
                    range_common_iterator_t<Rng>{ranges::begin(rng)},
                    range_common_iterator_t<Rng>{ranges::end(rng)})))(
                requires Range<Rng>)
            {
                using C = range_common_iterator_t<Rng>;
                return unwrap_reference(cont).insert(
                    p, C{ranges::begin(rng)}, C{ranges::end(rng)});
            }

            template<typename Cont, typename I, typename Rng>
            auto insert_impl(Cont &&cont_, I p, Rng &&rng, std::true_type) ->
                CPP_ret(decltype(unwrap_reference(cont_).insert(
                    begin(unwrap_reference(cont_)),
                    range_common_iterator_t<Rng>{ranges::begin(rng)},
                    range_common_iterator_t<Rng>{ranges::end(rng)})))(
                requires RandomAccessReservable<Cont> && SizedRange<Rng>)
            {
                using C = range_common_iterator_t<Rng>;
                auto &&cont = unwrap_reference(cont_);
                auto const delta = static_cast<range_size_t<Cont>>(ranges::size(rng));
                auto pos = insert_reserve_helper(cont, std::move(p), delta);
                return cont.insert(pos, C{ranges::begin(rng)}, C{ranges::end(rng)});
            }
        }
        /// \endcond

        template<typename Cont, typename P, typename I, typename S>
        auto insert(Cont &&cont, P p, I i, S j) ->
            CPP_ret(decltype(detail::insert_impl(
                static_cast<Cont &&>(cont),
                std::move(p),
                std::move(i),
                std::move(j),
                meta::bool_<RandomAccessReservable<Cont> && SizedSentinel<S, I>>{})))(
            requires LvalueContainerLike<Cont> && Iterator<P> &&
                Sentinel<S, I> && !Range<S>)
        {
            return detail::insert_impl(
                static_cast<Cont &&>(cont),
                std::move(p),
                std::move(i),
                std::move(j),
                meta::bool_<RandomAccessReservable<Cont> && SizedSentinel<S, I>>{});
        }

        template<typename Cont, typename I, typename Rng>
        auto insert(Cont &&cont, I p, Rng &&rng) ->
            CPP_ret(decltype(detail::insert_impl(
                static_cast<Cont &&>(cont),
                std::move(p),
                static_cast<Rng &&>(rng),
                meta::bool_<RandomAccessReservable<Cont> && SizedRange<Rng>>{})))(
            requires LvalueContainerLike<Cont> && Iterator<I> && Range<Rng>)
        {
            return detail::insert_impl(
                static_cast<Cont &&>(cont),
                std::move(p),
                static_cast<Rng &&>(rng),
                meta::bool_<RandomAccessReservable<Cont> && SizedRange<Rng>>{});
        }

        struct insert_fn
        {
            template<typename Rng, typename... Args>
            using insert_result_t =
                decltype(insert(std::declval<Rng>(), std::declval<Args>()...));

            template<typename Rng, typename T>
            auto operator()(Rng &&rng, T &&t) const ->
                CPP_ret(insert_result_t<Rng, T>)(
                    requires Range<Rng> && !Range<T> &&
                        Constructible<range_value_t<Rng>, T>)
            {
                return insert(static_cast<Rng &&>(rng), static_cast<T &&>(t));
            }

            template<typename Rng, typename Rng2>
            auto operator()(Rng &&rng, Rng2 &&rng2) const ->
                CPP_ret(insert_result_t<Rng, Rng2>)(
                    requires Range<Rng> && Range<Rng2>)
            {
                static_assert(!is_infinite<Rng>::value,
                    "Attempting to insert an infinite range into a container");
                return insert(static_cast<Rng &&>(rng), static_cast<Rng2 &&>(rng2));
            }

            template<typename Rng, typename T>
            auto operator()(Rng &&rng, std::initializer_list<T> rng2) const ->
                CPP_ret(insert_result_t<Rng, std::initializer_list<T> &>)(
                    requires Range<Rng>)
            {
                return insert(static_cast<Rng &&>(rng), rng2);
            }

            template<typename Rng, typename I, typename S>
            auto operator()(Rng &&rng, I i, S j) const ->
                CPP_ret(insert_result_t<Rng, I, S>)(
                    requires Range<Rng> && Sentinel<S, I> && !Range<S>)
            {
                return insert(static_cast<Rng &&>(rng), std::move(i), std::move(j));
            }

            template<typename Rng, typename I, typename T>
            auto operator()(Rng &&rng, I p, T &&t) const ->
                CPP_ret(insert_result_t<Rng, I, T>)(
                    requires Range<Rng> && Iterator<I> && !Range<T> &&
                        Constructible<range_value_t<Rng>, T>)
            {
                return insert(static_cast<Rng &&>(rng), std::move(p), static_cast<T &&>(t));
            }

            template<typename Rng, typename I, typename Rng2>
            auto operator()(Rng &&rng, I p, Rng2 &&rng2) const ->
                CPP_ret(insert_result_t<Rng, I, Rng2>)(
                    requires Range<Rng> && Iterator<I> && Range<Rng2>)
            {
                static_assert(!is_infinite<Rng>::value,
                    "Attempting to insert an infinite range into a container");
                return insert(static_cast<Rng &&>(rng), std::move(p), static_cast<Rng2 &&>(rng2));
            }

            template<typename Rng, typename I, typename T>
            auto operator()(Rng &&rng, I p, std::initializer_list<T> rng2) const ->
                CPP_ret(insert_result_t<Rng, I, std::initializer_list<T> &>)(
                    requires Range<Rng> && Iterator<I>)
            {
                return insert(static_cast<Rng &&>(rng), std::move(p), rng2);
            }

            template<typename Rng, typename I, typename N, typename T>
            auto operator()(Rng &&rng, I p, N n, T &&t) const ->
                CPP_ret(insert_result_t<Rng, I, N, T>)(
                    requires Range<Rng> && Iterator<I> && Integral<N> && !Range<T> &&
                        Constructible<range_value_t<Rng>, T>)
            {
                return insert(static_cast<Rng &&>(rng), std::move(p), n, static_cast<T &&>(t));
            }

            template<typename Rng, typename P, typename I, typename S>
            auto operator()(Rng &&rng, P p, I i, S j) const ->
                CPP_ret(insert_result_t<Rng, P, I, S>)(
                requires Range<Rng> && Iterator<P> && Sentinel<S, I> && !Range<S>)
            {
                return insert(static_cast<Rng &&>(rng), std::move(p), std::move(i),
                    std::move(j));
            }
        };
    }
    /// \endcond

    /// \ingroup group-actions
    RANGES_INLINE_VARIABLE(adl_insert_detail::insert_fn, insert)

    namespace action
    {
        using ranges::insert;
    }

    /// \ingroup group-concepts
    CPP_def
    (
        template(typename Rng, typename... Rest)
        (concept InsertableRange)(Rng, Rest...),
            requires (Rng &&rng, Rest &&... rest)
            (
                ranges::insert(
                    static_cast<Rng &&>(rng),
                    static_cast<Rest &&>(rest)...)
            ) &&
            Range<Rng>
    );
}

#endif
