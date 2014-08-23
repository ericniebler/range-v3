//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_VIEW_ALL_HPP
#define RANGES_V3_VIEW_ALL_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/meta.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T>
            iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
            view_all(T & t, concepts::Iterable*, concepts::IteratorRange*)
            {
                return {begin(t), end(t)};
            }

            template<typename T>
            iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
            view_all(T & t, concepts::Iterable*, concepts::SizedIteratorRange*)
            {
                return {begin(t), end(t)};
            }

            template<typename T>
            sized_iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
            view_all(T & t, concepts::SizedIterable*, concepts::IteratorRange*)
            {
                return {begin(t), end(t), size(t)};
            }

            template<typename T>
            iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
            view_all(T & t, concepts::SizedIterable*, concepts::SizedIteratorRange*)
            {
                RANGES_ASSERT(size(t) == iterator_range_size(begin(t), end(t)));
                return {begin(t), end(t)};
            }
        }

        namespace view
        {
            struct all_fn : bindable<all_fn>
            {
                /// If it's a range already, pass it though.
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>())>
                static T invoke(all_fn, T && t)
                {
                    return std::forward<T>(t);
                }

                /// If it is container-like, turn it into an iterator_range
                template<typename T,
                    typename I = range_iterator_t<T>,
                    typename S = range_sentinel_t<T>,
                    CONCEPT_REQUIRES_(Iterable<T>() && !Range<T>())>
                static auto invoke(all_fn, T && t) ->
                    decltype(detail::view_all(t, sized_iterable_concept<T>(), sized_iterator_range_concept<I, S>()))
                {
                    static_assert(std::is_lvalue_reference<T>::value,
                        "Cannot get a view of a temporary container");
                    return detail::view_all(t, sized_iterable_concept<T>(), sized_iterator_range_concept<I, S>());
                }

                // TODO handle char const * by turning it into a delimited range?
            };

            RANGES_CONSTEXPR all_fn all{};
        }

        template<typename Iterable>
        using range_view_all_t =
            meta_apply<std::decay, decltype(view::all(std::declval<Iterable>()))>;
    }
}

#endif
