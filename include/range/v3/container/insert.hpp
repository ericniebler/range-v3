// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_CONTAINER_INSERT_HPP
#define RANGES_V3_CONTAINER_INSERT_HPP

#include <utility>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/container/concepts.hpp>
#include <range/v3/view/bounded.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_insert_detail
        {
            template<typename Cont, typename T,
                CONCEPT_REQUIRES_(Container<Cont>())>
            auto insert(Cont & cont, T && t) ->
                decltype(cont.insert(std::forward<T>(t)))
            {
                return cont.insert(std::forward<T>(t));
            }

            template<typename Cont, typename I, typename S,
                typename C = common_iterator<I, S>,
                CONCEPT_REQUIRES_(Container<Cont>() && IteratorRange<I, S>())>
            auto insert(Cont & cont, I i, S j) ->
                decltype(cont.insert(C{i}, C{j}))
            {
                return cont.insert(C{i}, C{j});
            }

            template<typename Cont, typename Rng,
                typename C = range_common_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterable<Rng>())>
            auto insert(Cont & cont, Rng && rng) ->
                decltype(cont.insert(C{begin(rng)}, C{end(rng)}))
            {
                static_assert(!is_infinite<Rng>::value,
                    "Attempting to insert an infinite range into a container");
                return cont.insert(C{begin(rng)}, C{end(rng)});
            }

            template<typename Cont, typename I, typename T,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterator<I>())>
            auto insert(Cont & cont, I p, T && t) ->
                decltype(cont.insert(p, std::forward<T>(t)))
            {
                return cont.insert(p, std::forward<T>(t));
            }

            template<typename Cont, typename I, typename N, typename T,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterator<I>() && Integral<N>())>
            auto insert(Cont & cont, I p, N n, T && t) ->
                decltype(cont.insert(p, n, std::forward<T>(t)))
            {
                return cont.insert(p, n, std::forward<T>(t));
            }

            template<typename Cont, typename P, typename I, typename S,
                typename C = common_iterator<I, S>,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterator<P>() && IteratorRange<I, S>())>
            auto insert(Cont & cont, P p, I i, S j) ->
                decltype(cont.insert(p, C{i}, C{j}))
            {
                return cont.insert(p, C{i}, C{j});
            }

            template<typename Cont, typename I, typename Rng,
                typename C = range_common_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterator<I>() && Iterable<Rng>())>
            auto insert(Cont & cont, I p, Rng && rng) ->
                decltype(cont.insert(p, C{begin(rng)}, C{end(rng)}))
            {
                static_assert(!is_infinite<Rng>::value,
                    "Attempting to insert an infinite range into a container");
                return cont.insert(p, C{begin(rng)}, C{end(rng)});
            }

            // TODO bindable? view::ints | view::take(10) | container::insert(v, v.begin());
            struct insert_fn
            {
                template<typename Cont, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>())>
                auto operator()(Cont & cont, T && t) const ->
                    decltype(insert(cont, std::forward<T>(t)))
                {
                    return insert(cont, std::forward<T>(t));
                }

                template<typename Cont, typename I, typename S,
                    CONCEPT_REQUIRES_(Container<Cont>() && IteratorRange<I, S>())>
                auto operator()(Cont & cont, I i, S j) const ->
                    decltype(insert(cont, i, j))
                {
                    return insert(cont, i, j);
                }

                template<typename Cont, typename I, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>() && Iterator<I>())>
                auto operator()(Cont & cont, I p, T && t) const ->
                    decltype(insert(cont, p, std::forward<T>(t)))
                {
                    return insert(cont, p, std::forward<T>(t));
                }

                template<typename Cont, typename I, typename N, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>() && Iterator<I>() && Integral<N>())>
                auto operator()(Cont & cont, I p, N n, T && t) const ->
                    decltype(insert(cont, p, n, std::forward<T>(t)))
                {
                    return insert(cont, p, n, std::forward<T>(t));
                }

                template<typename Cont, typename P, typename I, typename S,
                    CONCEPT_REQUIRES_(Container<Cont>() && Iterator<P>() && IteratorRange<I, S>())>
                auto operator()(Cont & cont, P p, I i, S j) const ->
                    decltype(insert(cont, p, i, j))
                {
                    return insert(cont, p, i, j);
                }

                template<typename Cont, typename I, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>() && Iterator<I>())>
                auto operator()(Cont & cont, I p, std::initializer_list<T> rng) const ->
                    decltype(insert(cont, p, rng.begin(), rng.end()))
                {
                    return insert(cont, p, rng.begin(), rng.end());
                }
            };
        }

        namespace container
        {
            RANGES_CONSTEXPR adl_insert_detail::insert_fn insert{};
        }

        namespace concepts
        {
            struct ExtensibleContainer
              : refines<Container(_1)>
            {
                template<typename Cont, typename...Rest>
                auto requires_(Cont && cont, Rest &&... rest) -> decltype(
                    concepts::valid_expr(
                        (container::insert(cont, std::forward<Rest>(rest)...), 42)
                    ));
            };
        }

        template<typename Cont, typename...Rest>
        using ExtensibleContainer = concepts::models<concepts::ExtensibleContainer, Cont, Rest...>;
    }
}

#endif
