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
        template<typename Cont, typename... Args>
        using insert_result_t = decltype(
            unwrap_reference(std::declval<Cont>()).insert(std::declval<Args>()...));

        template<typename Cont, typename T>
        auto insert(Cont && cont, T && t) -> CPP_ret(insert_result_t<Cont &, T>)( //
            requires lvalue_container_like<Cont> &&
            (!range<T> && constructible_from<range_value_t<Cont>, T>))
        {
            return unwrap_reference(cont).insert(static_cast<T &&>(t));
        }

        template<typename Cont, typename I, typename S>
        auto insert(Cont && cont, I i, S j)
            -> CPP_ret(insert_result_t<Cont &, detail::cpp17_iterator_t<I, S>,
                                       detail::cpp17_iterator_t<I, S>>)( //
                requires lvalue_container_like<Cont> && sentinel_for<S, I> && (!range<S>))
        {
            return unwrap_reference(cont).insert(detail::cpp17_iterator_t<I, S>{i},
                                                 detail::cpp17_iterator_t<I, S>{j});
        }

        template<typename Cont, typename Rng>
        auto insert(Cont && cont, Rng && rng)
            -> CPP_ret(insert_result_t<Cont &, detail::range_cpp17_iterator_t<Rng>,
                                       detail::range_cpp17_iterator_t<Rng>>)( //
                requires lvalue_container_like<Cont> && range<Rng>)
        {
            return unwrap_reference(cont).insert(
                detail::range_cpp17_iterator_t<Rng>{ranges::begin(rng)},
                detail::range_cpp17_iterator_t<Rng>{ranges::end(rng)});
        }

        template<typename Cont, typename I, typename T>
        auto insert(Cont && cont, I p, T && t)
            -> CPP_ret(insert_result_t<Cont &, I, T>)( //
                requires lvalue_container_like<Cont> && input_iterator<I> &&
                (!range<T> && constructible_from<range_value_t<Cont>, T>))
        {
            return unwrap_reference(cont).insert(p, static_cast<T &&>(t));
        }

        template<typename Cont, typename I, typename N, typename T>
        auto insert(Cont && cont, I p, N n, T && t)
            -> CPP_ret(insert_result_t<Cont &, I, N, T>)( //
                requires lvalue_container_like<Cont> && input_iterator<I> &&
                    integral<N> && constructible_from<range_value_t<Cont>, T>)
        {
            return unwrap_reference(cont).insert(p, n, static_cast<T &&>(t));
        }

        /// \cond
        namespace detail
        {
            using ranges::detail::cpp17_iterator_t;
            using ranges::detail::range_cpp17_iterator_t;

            template<typename Cont, typename P>
            auto insert_reserve_helper(Cont & cont, P const p,
                                       range_size_t<Cont> const delta)
                -> CPP_ret(iterator_t<Cont>)( //
                    requires container<Cont> && input_iterator<P> &&
                        random_access_reservable<Cont>)
            {
                auto const old_size = ranges::size(cont);
                auto const max_size = cont.max_size();
                RANGES_EXPECT(delta <= max_size - old_size);
                auto const new_size = old_size + delta;
                auto const old_capacity = cont.capacity();
                auto const index = p - ranges::begin(cont);
                if(old_capacity < new_size)
                {
                    auto const new_capacity =
                        (old_capacity <= max_size / 3 * 2)
                            ? ranges::max(old_capacity + old_capacity / 2, new_size)
                            : max_size;
                    cont.reserve(new_capacity);
                }
                return ranges::begin(cont) + index;
            }

            template<typename Cont, typename P, typename I, typename S>
            auto insert_impl(Cont && cont, P p, I i, S j, std::false_type)
                -> CPP_ret(decltype(unwrap_reference(cont).insert(
                    p, cpp17_iterator_t<I, S>{i}, cpp17_iterator_t<I, S>{j})))( //
                    requires sentinel_for<S, I> && (!range<S>))
            {
                using C = cpp17_iterator_t<I, S>;
                return unwrap_reference(cont).insert(p, C{i}, C{j});
            }

            template<typename Cont, typename P, typename I, typename S>
            auto insert_impl(Cont && cont_, P p, I i, S j, std::true_type)
                -> CPP_ret(decltype(unwrap_reference(cont_).insert(
                    ranges::begin(unwrap_reference(cont_)), cpp17_iterator_t<I, S>{i},
                    cpp17_iterator_t<I, S>{j})))( //
                    requires sized_sentinel_for<S, I> && random_access_reservable<Cont> &&
                    (!range<S>))
            {
                using C = cpp17_iterator_t<I, S>;
                auto && cont = unwrap_reference(cont_);
                auto const delta = static_cast<range_size_t<Cont>>(j - i);
                auto pos = insert_reserve_helper(cont, std::move(p), delta);
                return cont.insert(pos, C{std::move(i)}, C{std::move(j)});
            }

            template<typename Cont, typename I, typename Rng>
            auto insert_impl(Cont && cont, I p, Rng && rng, std::false_type)
                -> CPP_ret(decltype(unwrap_reference(cont).insert(
                    p, range_cpp17_iterator_t<Rng>{ranges::begin(rng)},
                    range_cpp17_iterator_t<Rng>{ranges::end(rng)})))( //
                    requires range<Rng>)
            {
                using C = range_cpp17_iterator_t<Rng>;
                return unwrap_reference(cont).insert(
                    p, C{ranges::begin(rng)}, C{ranges::end(rng)});
            }

            template<typename Cont, typename I, typename Rng>
            auto insert_impl(Cont && cont_, I p, Rng && rng, std::true_type)
                -> CPP_ret(decltype(unwrap_reference(cont_).insert(
                    begin(unwrap_reference(cont_)),
                    range_cpp17_iterator_t<Rng>{ranges::begin(rng)},
                    range_cpp17_iterator_t<Rng>{ranges::end(rng)})))( //
                    requires random_access_reservable<Cont> && sized_range<Rng>)
            {
                using C = range_cpp17_iterator_t<Rng>;
                auto && cont = unwrap_reference(cont_);
                auto const delta = static_cast<range_size_t<Cont>>(ranges::size(rng));
                auto pos = insert_reserve_helper(cont, std::move(p), delta);
                return cont.insert(pos, C{ranges::begin(rng)}, C{ranges::end(rng)});
            }
        } // namespace detail
        /// \endcond

        template<typename Cont, typename P, typename I, typename S>
        auto insert(Cont && cont, P p, I i, S j) -> CPP_ret(decltype(detail::insert_impl(
            static_cast<Cont &&>(cont), std::move(p), std::move(i), std::move(j),
            meta::bool_<random_access_reservable<Cont> &&
                        sized_sentinel_for<S, I>>{})))( //
            requires lvalue_container_like<Cont> && input_iterator<P> &&
                sentinel_for<S, I> &&
            (!range<S>))
        {
            return detail::insert_impl(static_cast<Cont &&>(cont),
                                       std::move(p),
                                       std::move(i),
                                       std::move(j),
                                       meta::bool_ < random_access_reservable<Cont> &&
                                           sized_sentinel_for<S, I>> {});
        }

        template<typename Cont, typename I, typename Rng>
        auto insert(Cont && cont, I p, Rng && rng)
            -> CPP_ret(decltype(detail::insert_impl(
                static_cast<Cont &&>(cont), std::move(p), static_cast<Rng &&>(rng),
                meta::bool_<random_access_reservable<Cont> && sized_range<Rng>>{})))( //
                requires lvalue_container_like<Cont> && input_iterator<I> && range<Rng>)
        {
            return detail::insert_impl(static_cast<Cont &&>(cont),
                                       std::move(p),
                                       static_cast<Rng &&>(rng),
                                       meta::bool_ < random_access_reservable<Cont> &&
                                           sized_range<Rng>> {});
        }

        struct insert_fn
        {
            template<typename Rng, typename... Args>
            using insert_result_t =
                decltype(insert(std::declval<Rng>(), std::declval<Args>()...));

            template<typename Rng, typename T>
            auto operator()(Rng && rng, T && t) const
                -> CPP_ret(insert_result_t<Rng, T>)( //
                    requires range<Rng> &&
                    (!range<T>)&&constructible_from<range_value_t<Rng>, T>)
            {
                return insert(static_cast<Rng &&>(rng), static_cast<T &&>(t));
            }

            template<typename Rng, typename Rng2>
            auto operator()(Rng && rng, Rng2 && rng2) const
                -> CPP_ret(insert_result_t<Rng, Rng2>)( //
                    requires range<Rng> && range<Rng2>)
            {
                static_assert(!is_infinite<Rng>::value,
                              "Attempting to insert an infinite range into a container");
                return insert(static_cast<Rng &&>(rng), static_cast<Rng2 &&>(rng2));
            }

            template<typename Rng, typename T>
            auto operator()(Rng && rng, std::initializer_list<T> rng2) const
                -> CPP_ret(insert_result_t<Rng, std::initializer_list<T> &>)( //
                    requires range<Rng>)
            {
                return insert(static_cast<Rng &&>(rng), rng2);
            }

            template<typename Rng, typename I, typename S>
            auto operator()(Rng && rng, I i, S j) const
                -> CPP_ret(insert_result_t<Rng, I, S>)( //
                    requires range<Rng> && sentinel_for<S, I> && (!range<S>))
            {
                return insert(static_cast<Rng &&>(rng), std::move(i), std::move(j));
            }

            template<typename Rng, typename I, typename T>
            auto operator()(Rng && rng, I p, T && t) const
                -> CPP_ret(insert_result_t<Rng, I, T>)( //
                    requires range<Rng> && input_iterator<I> &&
                    (!range<T>)&&constructible_from<range_value_t<Rng>, T>)
            {
                return insert(
                    static_cast<Rng &&>(rng), std::move(p), static_cast<T &&>(t));
            }

            template<typename Rng, typename I, typename Rng2>
            auto operator()(Rng && rng, I p, Rng2 && rng2) const
                -> CPP_ret(insert_result_t<Rng, I, Rng2>)( //
                    requires range<Rng> && input_iterator<I> && range<Rng2>)
            {
                static_assert(!is_infinite<Rng>::value,
                              "Attempting to insert an infinite range into a container");
                return insert(
                    static_cast<Rng &&>(rng), std::move(p), static_cast<Rng2 &&>(rng2));
            }

            template<typename Rng, typename I, typename T>
            auto operator()(Rng && rng, I p, std::initializer_list<T> rng2) const
                -> CPP_ret(insert_result_t<Rng, I, std::initializer_list<T> &>)( //
                    requires range<Rng> && input_iterator<I>)
            {
                return insert(static_cast<Rng &&>(rng), std::move(p), rng2);
            }

            template<typename Rng, typename I, typename N, typename T>
            auto operator()(Rng && rng, I p, N n, T && t) const
                -> CPP_ret(insert_result_t<Rng, I, N, T>)( //
                    requires range<Rng> && input_iterator<I> && integral<N> &&
                    (!range<T>)&&constructible_from<range_value_t<Rng>, T>)
            {
                return insert(
                    static_cast<Rng &&>(rng), std::move(p), n, static_cast<T &&>(t));
            }

            template<typename Rng, typename P, typename I, typename S>
            auto operator()(Rng && rng, P p, I i, S j) const
                -> CPP_ret(insert_result_t<Rng, P, I, S>)( //
                    requires range<Rng> && input_iterator<P> && sentinel_for<S, I> &&
                    (!range<S>))
            {
                return insert(
                    static_cast<Rng &&>(rng), std::move(p), std::move(i), std::move(j));
            }
        };
    } // namespace adl_insert_detail
    /// \endcond

    /// \ingroup group-actions
    RANGES_INLINE_VARIABLE(adl_insert_detail::insert_fn, insert)

    namespace actions
    {
        using ranges::insert;
    }
} // namespace ranges

#endif
