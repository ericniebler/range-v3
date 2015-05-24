/// \file
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

#ifndef RANGES_V3_ACTION_INSERT_HPP
#define RANGES_V3_ACTION_INSERT_HPP

#include <utility>
#include <functional>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_insert_detail
        {
            template<typename Cont, typename T,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Constructible<range_value_t<Cont>, T &&>())>
            auto insert(Cont && cont, T && t) ->
                decltype(unwrap_reference(cont).insert(std::forward<T>(t)))
            {
                return unwrap_reference(cont).insert(std::forward<T>(t));
            }

            template<typename Cont, typename I, typename S,
                typename C = common_iterator<I, S>,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && IteratorRange<I, S>())>
            auto insert(Cont && cont, I i, S j) ->
                decltype(unwrap_reference(cont).insert(C{i}, C{j}))
            {
                return unwrap_reference(cont).insert(C{i}, C{j});
            }

            template<typename Cont, typename Rng,
                typename C = range_common_iterator_t<Rng>,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Range<Rng>())>
            auto insert(Cont && cont, Rng && rng) ->
                decltype(unwrap_reference(cont).insert(C{begin(rng)}, C{end(rng)}))
            {
                return unwrap_reference(cont).insert(C{begin(rng)}, C{end(rng)});
            }

            template<typename Cont, typename I, typename T,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Iterator<I>() &&
                    Constructible<range_value_t<Cont>, T &&>())>
            auto insert(Cont && cont, I p, T && t) ->
                decltype(unwrap_reference(cont).insert(p, std::forward<T>(t)))
            {
                return unwrap_reference(cont).insert(p, std::forward<T>(t));
            }

            template<typename Cont, typename I, typename N, typename T,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Iterator<I>() && Integral<N>() &&
                    Constructible<range_value_t<Cont>, T &&>())>
            auto insert(Cont && cont, I p, N n, T && t) ->
                decltype(unwrap_reference(cont).insert(p, n, std::forward<T>(t)))
            {
                return unwrap_reference(cont).insert(p, n, std::forward<T>(t));
            }

            template<typename Cont, typename P, typename I, typename S,
                typename C = common_iterator<I, S>,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Iterator<P>() && IteratorRange<I, S>())>
            auto insert(Cont && cont, P p, I i, S j) ->
                decltype(unwrap_reference(cont).insert(p, C{i}, C{j}))
            {
                return unwrap_reference(cont).insert(p, C{i}, C{j});
            }

            template<typename T, typename A, typename P, typename I, typename S,
                typename C = common_iterator<I, S>,
                typename CP = typename std::vector<T, A>::const_iterator,
                CONCEPT_REQUIRES_(Convertible<P, CP>() && SizedIteratorRange<I, S>())>
            auto insert(std::vector<T, A>& vec, P p, I i, S j) ->
                decltype(vec.insert(begin(vec), C{i}, C{j}))
            {
                auto const index = CP(p) - vec.begin();
                vec.reserve(vec.size() + (j - i));
                return vec.insert(begin(vec) + index, C{i}, C{j});
            }

            template<typename Cont, typename I, typename Rng,
                typename C = range_common_iterator_t<Rng>,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Iterator<I>() && Range<Rng>())>
            auto insert(Cont && cont, I p, Rng && rng) ->
                decltype(unwrap_reference(cont).insert(p, C{begin(rng)}, C{end(rng)}))
            {
                return unwrap_reference(cont).insert(p, C{begin(rng)}, C{end(rng)});
            }

            template<typename T, typename A, typename I, typename Rng,
                typename C = range_common_iterator_t<Rng>,
                typename CI = typename std::vector<T, A>::const_iterator,
                CONCEPT_REQUIRES_(Convertible<I, CI>() && SizedRange<Rng>())>
            auto insert(std::vector<T, A>& vec, I p, Rng && rng) ->
                decltype(vec.insert(begin(vec), C{begin(rng)}, C{end(rng)}))
            {
                auto const index = CI(p) - vec.begin();
                vec.reserve(vec.size() + size(rng));
                return vec.insert(begin(vec) + index, C{begin(rng)}, C{end(rng)});
            }

            struct insert_fn
            {
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(Range<Rng>() && Constructible<range_value_t<Rng>, T &&>())>
                auto operator()(Rng && rng, T && t) const ->
                    decltype(insert(std::forward<Rng>(rng), std::forward<T>(t)))
                {
                    return insert(std::forward<Rng>(rng), std::forward<T>(t));
                }

                template<typename Rng, typename Rng2,
                    CONCEPT_REQUIRES_(Range<Rng>() && Range<Rng2>())>
                auto operator()(Rng && rng, Rng2 && rng2) const ->
                    decltype(insert(std::forward<Rng>(rng), std::forward<Rng2>(rng2)))
                {
                    static_assert(!is_infinite<Rng>::value,
                        "Attempting to insert an infinite range into a container");
                    return insert(std::forward<Rng>(rng), std::forward<Rng2>(rng2));
                }

                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(Range<Rng>())>
                auto operator()(Rng && rng, std::initializer_list<T> rng2) const ->
                    decltype(insert(std::forward<Rng>(rng), rng2))
                {
                    return insert(std::forward<Rng>(rng), rng2);
                }

                template<typename Rng, typename I, typename S,
                    CONCEPT_REQUIRES_(Range<Rng>() && IteratorRange<I, S>())>
                auto operator()(Rng && rng, I i, S j) const ->
                    decltype(insert(std::forward<Rng>(rng), i, j))
                {
                    return insert(std::forward<Rng>(rng), i, j);
                }

                template<typename Rng, typename I, typename T,
                    CONCEPT_REQUIRES_(Range<Rng>() && Iterator<I>() &&
                        Constructible<range_value_t<Rng>, T &&>())>
                auto operator()(Rng && rng, I p, T && t) const ->
                    decltype(insert(std::forward<Rng>(rng), p, std::forward<T>(t)))
                {
                    return insert(std::forward<Rng>(rng), p, std::forward<T>(t));
                }

                template<typename Rng, typename I, typename Rng2,
                    CONCEPT_REQUIRES_(Range<Rng>() && Iterator<I>() && Range<Rng2>())>
                auto operator()(Rng && rng, I p, Rng2 && rng2) const ->
                    decltype(insert(std::forward<Rng>(rng), p, std::forward<Rng2>(rng2)))
                {
                    static_assert(!is_infinite<Rng>::value,
                        "Attempting to insert an infinite range into a container");
                    return insert(std::forward<Rng>(rng), p, std::forward<Rng2>(rng2));
                }

                template<typename Rng, typename I, typename T,
                    CONCEPT_REQUIRES_(Range<Rng>() && Iterator<I>())>
                auto operator()(Rng && rng, I p, std::initializer_list<T> rng2) const ->
                    decltype(insert(std::forward<Rng>(rng), p, rng2))
                {
                    return insert(std::forward<Rng>(rng), p, rng2);
                }

                template<typename Rng, typename I, typename N, typename T,
                    CONCEPT_REQUIRES_(Range<Rng>() && Iterator<I>() && Integral<N>()
                        && Constructible<range_value_t<Rng>, T &&>())>
                auto operator()(Rng && rng, I p, N n, T && t) const ->
                    decltype(insert(std::forward<Rng>(rng), p, n, std::forward<T>(t)))
                {
                    return insert(std::forward<Rng>(rng), p, n, std::forward<T>(t));
                }

                template<typename Rng, typename P, typename I, typename S,
                    CONCEPT_REQUIRES_(Range<Rng>() && Iterator<P>() && IteratorRange<I, S>())>
                auto operator()(Rng && rng, P p, I i, S j) const ->
                    decltype(insert(std::forward<Rng>(rng), p, i, j))
                {
                    return insert(std::forward<Rng>(rng), p, i, j);
                }
            };
        }
        /// \endcond

        /// \ingroup group-actions
        namespace
        {
            constexpr auto&& insert = static_const<adl_insert_detail::insert_fn>::value;
        }

        namespace action
        {
            using ranges::insert;
        }

        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct InsertableRange
              : refines<Range(_1)>
            {
                template<typename Rng, typename...Rest>
                auto requires_(Rng&& rng, Rest&&... rest) -> decltype(
                    concepts::valid_expr(
                        (ranges::insert(std::forward<Rng>(rng), std::forward<Rest>(rest)...), 42)
                    ));
            };
        }

        template<typename Rng, typename...Rest>
        using InsertableRange = concepts::models<concepts::InsertableRange, Rng, Rest...>;
        /// @}
    }
}

#endif
