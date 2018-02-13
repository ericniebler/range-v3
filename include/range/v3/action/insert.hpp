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
#include <range/v3/range_traits.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_insert_detail
        {
            CONCEPT_template(typename Cont, typename T)(
                requires LvalueContainerLike<Cont>() &&
                    Constructible<range_value_type_t<Cont>, T>())
            (auto) insert(Cont &&cont, T &&t)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                unwrap_reference(cont).insert(static_cast<T &&>(t))
            )

            CONCEPT_template(typename Cont, typename I, typename S,
                typename C = common_iterator_t<I, S>)(
                requires LvalueContainerLike<Cont>() && Sentinel<S, I>() && !Range<S>())
            (auto) insert(Cont &&cont, I i, S j)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                unwrap_reference(cont).insert(C{i}, C{j})
            )

            CONCEPT_template(typename Cont, typename Rng,
                typename C = range_common_iterator_t<Rng>)(
                requires LvalueContainerLike<Cont>() && Range<Rng>())
            (auto) insert(Cont &&cont, Rng &&rng)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                unwrap_reference(cont).insert(C{ranges::begin(rng)}, C{ranges::end(rng)})
            )

            CONCEPT_template(typename Cont, typename I, typename T)(
                requires LvalueContainerLike<Cont>() && Iterator<I>() &&
                    Constructible<range_value_type_t<Cont>, T>())
            (auto) insert(Cont &&cont, I p, T &&t)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                unwrap_reference(cont).insert(p, static_cast<T &&>(t))
            )

            CONCEPT_template(typename Cont, typename I, typename N, typename T)(
                requires LvalueContainerLike<Cont>() && Iterator<I>() && Integral<N>() &&
                    Constructible<range_value_type_t<Cont>, T>())
            (auto) insert(Cont &&cont, I p, N n, T &&t)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                unwrap_reference(cont).insert(p, n, static_cast<T &&>(t))
            )

            /// \cond
            namespace detail
            {
                CONCEPT_template(typename Cont, typename P)(
                    requires Container<Cont>() && Iterator<P>() &&
                        RandomAccessReservable<Cont>())
                (iterator_t<Cont>) insert_reserve_helper(
                    Cont &cont, P const p, range_size_type_t<Cont> const delta)
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

                CONCEPT_template(typename Cont, typename P, typename I, typename S,
                    typename C = common_iterator_t<I, S>)(
                    requires Sentinel<S, I>() && !Range<S>())
                (auto) insert_impl(Cont &&cont, P p, I i, S j, std::false_type)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    unwrap_reference(cont).insert(p, C{i}, C{j})
                )

                CONCEPT_template(typename Cont, typename P, typename I, typename S,
                    typename C = common_iterator_t<I, S>)(
                    requires SizedSentinel<S, I>() && RandomAccessReservable<Cont>() && !Range<S>())
                (auto) insert_impl(Cont &&cont_, P p, I i, S j, std::true_type) ->
                    decltype(unwrap_reference(cont_).insert(
                        ranges::begin(unwrap_reference(cont_)), C{i}, C{j}))
                {
                    auto &&cont = unwrap_reference(cont_);
                    auto const delta = static_cast<range_size_type_t<Cont>>(j - i);
                    auto pos = insert_reserve_helper(cont, std::move(p), delta);
                    return cont.insert(pos, C{std::move(i)}, C{std::move(j)});
                }

                CONCEPT_template(typename Cont, typename I, typename Rng,
                    typename C = range_common_iterator_t<Rng>)(
                    requires Range<Rng>())
                (auto) insert_impl(Cont &&cont, I p, Rng &&rng, std::false_type)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    unwrap_reference(cont).insert(p, C{ranges::begin(rng)}, C{ranges::end(rng)})
                )

                CONCEPT_template(typename Cont, typename I, typename Rng,
                    typename C = range_common_iterator_t<Rng>)(
                    requires RandomAccessReservable<Cont>() && SizedRange<Rng>())
                (auto) insert_impl(Cont &&cont_, I p, Rng &&rng, std::true_type) ->
                    decltype(unwrap_reference(cont_).insert(
                        begin(unwrap_reference(cont_)), C{ranges::begin(rng)}, C{ranges::end(rng)}))
                {
                    auto &&cont = unwrap_reference(cont_);
                    auto const delta = static_cast<range_size_type_t<Cont>>(ranges::size(rng));
                    auto pos = insert_reserve_helper(cont, std::move(p), delta);
                    return cont.insert(pos, C{ranges::begin(rng)}, C{ranges::end(rng)});
                }
            }
            /// \endcond

            CONCEPT_template(typename Cont, typename P, typename I, typename S,
                typename C = common_iterator_t<I, S>)(
                requires LvalueContainerLike<Cont>() && Iterator<P>() &&
                    Sentinel<S, I>() && !Range<S>())
            (auto) insert(Cont &&cont, P p, I i, S j)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                detail::insert_impl(
                    static_cast<Cont &&>(cont),
                    std::move(p),
                    std::move(i),
                    std::move(j),
                    RandomAccessReservable<Cont>() && SizedSentinel<S, I>())
            )

            CONCEPT_template(typename Cont, typename I, typename Rng,
                typename C = range_common_iterator_t<Rng>)(
                requires LvalueContainerLike<Cont>() && Iterator<I>() && Range<Rng>())
            (auto) insert(Cont &&cont, I p, Rng &&rng)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                detail::insert_impl(
                    static_cast<Cont &&>(cont),
                    std::move(p),
                    static_cast<Rng &&>(rng),
                    RandomAccessReservable<Cont>() && SizedRange<Rng>())
            )

            struct insert_fn
            {
                CONCEPT_template(typename Rng, typename T)(
                    requires Range<Rng>() && Constructible<range_value_type_t<Rng>, T>())
                (auto) operator()(Rng &&rng, T &&t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), static_cast<T &&>(t))
                )

                CONCEPT_template(typename Rng, typename Rng2)(
                    requires Range<Rng>() && Range<Rng2>())
                (auto) operator()(Rng &&rng, Rng2 &&rng2) const ->
                    decltype(insert(static_cast<Rng &&>(rng), static_cast<Rng2 &&>(rng2)))
                {
                    static_assert(!is_infinite<Rng>::value,
                        "Attempting to insert an infinite range into a container");
                    return insert(static_cast<Rng &&>(rng), static_cast<Rng2 &&>(rng2));
                }

                CONCEPT_template(typename Rng, typename T)(
                    requires Range<Rng>())
                (auto) operator()(Rng &&rng, std::initializer_list<T> rng2) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), rng2)
                )

                CONCEPT_template(typename Rng, typename I, typename S)(
                    requires Range<Rng>() && Sentinel<S, I>() && !Range<S>())
                (auto) operator()(Rng &&rng, I i, S j) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), std::move(i), std::move(j))
                )

                CONCEPT_template(typename Rng, typename I, typename T)(
                    requires Range<Rng>() && Iterator<I>() &&
                        Constructible<range_value_type_t<Rng>, T>())
                (auto) operator()(Rng &&rng, I p, T &&t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), std::move(p), static_cast<T &&>(t))
                )

                CONCEPT_template(typename Rng, typename I, typename Rng2)(
                    requires Range<Rng>() && Iterator<I>() && Range<Rng2>())
                (decltype(insert(std::declval<Rng>(), std::declval<I>(), std::declval<Rng2>())))
                operator()(Rng &&rng, I p, Rng2 &&rng2) const
                {
                    static_assert(!is_infinite<Rng>::value,
                        "Attempting to insert an infinite range into a container");
                    return insert(static_cast<Rng &&>(rng), std::move(p), static_cast<Rng2 &&>(rng2));
                }

                CONCEPT_template(typename Rng, typename I, typename T)(
                    requires Range<Rng>() && Iterator<I>())
                (auto) operator()(Rng &&rng, I p, std::initializer_list<T> rng2) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), std::move(p), rng2)
                )

                CONCEPT_template(typename Rng, typename I, typename N, typename T)(
                    requires Range<Rng>() && Iterator<I>() && Integral<N>() &&
                        Constructible<range_value_type_t<Rng>, T>())
                (auto) operator()(Rng &&rng, I p, N n, T &&t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), std::move(p), n, static_cast<T &&>(t))
                )

                CONCEPT_template(typename Rng, typename P, typename I, typename S)(
                    requires Range<Rng>() && Iterator<P>() && Sentinel<S, I>() &&
                                      !Range<S>())
                (auto) operator()(Rng &&rng, P p, I i, S j) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    insert(static_cast<Rng &&>(rng), std::move(p), std::move(i), std::move(j))
                )
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
        CONCEPT_def
        (
            template(typename Rng, typename... Rest)
            (concept InsertableRange)(Rng, Rest...),
                requires (Rng &&rng, Rest &&... rest)
                {
                    ((void)ranges::insert(
                        static_cast<Rng &&>(rng),
                        static_cast<Rest &&>(rest)...), 42)
                } &&
                Range<Rng>()
        );
    }
}

#endif
