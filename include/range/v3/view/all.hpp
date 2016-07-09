/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_ALL_HPP
#define RANGES_V3_VIEW_ALL_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct all_fn : pipeable<all_fn>
            {
            private:
                template<typename T>
                static iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
                from_container(T & t, concepts::Range*, concepts::Sentinel*)
                {
                    return {begin(t), end(t)};
                }

                template<typename T>
                static sized_iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
                from_container(T & t, concepts::SizedRange*, concepts::Sentinel*)
                {
                    return {begin(t), end(t), size(t)};
                }

                template<typename T>
                static iterator_range<range_iterator_t<T>, range_sentinel_t<T>>
                from_container(T & t, concepts::SizedRange*, concepts::SizedSentinel*)
                {
                    RANGES_ASSERT(size(t) == size(begin(t), end(t)));
                    return {begin(t), end(t)};
                }

                /// If it's a view already, pass it though.
                template<typename T>
                static T from_range(T && t, concepts::View*)
                {
                    return std::forward<T>(t);
                }

                /// If it is container-like, turn it into a view, being careful
                /// to preserve the Sized-ness of the range.
                template<typename T,
                    CONCEPT_REQUIRES_(!View<T>()),
                    typename I = range_iterator_t<T>,
                    typename S = range_sentinel_t<T>,
                    typename SIC = sized_range_concept<T>,
                    typename SIRC = sized_sentinel_concept<S, I>>
                static auto from_range(T && t, concepts::Range*) ->
                    decltype(all_fn::from_container(t, SIC(), SIRC()))
                {
                    static_assert(std::is_lvalue_reference<T>::value, "Cannot get a view of a temporary container");
                    return all_fn::from_container(t, SIC(), SIRC());
                }

                // TODO handle char const * by turning it into a delimited range?

            public:
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>())>
                auto operator()(T && t) const ->
                    decltype(all_fn::from_range(std::forward<T>(t), view_concept<T>()))
                {
                    return all_fn::from_range(std::forward<T>(t), view_concept<T>());
                }

                template<typename T,
                    CONCEPT_REQUIRES_(Range<T &>())>
                ranges::reference_wrapper<T> operator()(std::reference_wrapper<T> ref) const
                {
                    return ranges::ref(ref.get());
                }
            };

            /// \relates all_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(all_fn, all)

            template<typename Rng>
            using all_t =
                meta::_t<std::decay<decltype(all(std::declval<Rng>()))>>;
        }
        /// @}
    }
}

#endif
