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

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            struct all_fn : bindable<all_fn>
            {
                /// If it's an iterable already, pass it though.
                template<typename T,
                    CONCEPT_REQUIRES_(ConvertibleToIterable<T>() &&
                        is_iterable<T>::value)>
                static T invoke(all_fn, T && t)
                {
                    return std::forward<T>(t);
                }

                /// If it is container-like, turn it into an iterator_range
                template<typename T,
                    CONCEPT_REQUIRES_(ConvertibleToIterable<T>() &&
                        !is_iterable<T>::value)>
                static auto invoke(all_fn, T && t) ->
                    decltype(detail::container_view_all(t, detail::convertible_to_sized_iterable_concept<T>()))
                {
                    static_assert(!std::is_lvalue_reference<T>::value,
                        "Cannot get a view of a temporary container");
                    return detail::container_view_all(t, detail::convertible_to_sized_iterable_concept<T>());
                }

                // TODO handle char const * by turning it into a delimited range
            };

            RANGES_CONSTEXPR all_fn all{};
        }

        template<typename ConvertibleToIterable>
        using range_view_all_t = decltype(view::all(std::declval<ConvertibleToIterable>()));
    }
}

#endif
