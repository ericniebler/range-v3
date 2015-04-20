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
#include <range/v3/range.hpp>
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
                static RANGES_RELAXED_CONSTEXPR range<range_iterator_t<T>, range_sentinel_t<T>>
                from_container(T & t, concepts::Iterable*, concepts::IteratorRange*)
                {
                    return {begin(t), end(t)};
                }

                template<typename T>
                static RANGES_RELAXED_CONSTEXPR sized_range<range_iterator_t<T>, range_sentinel_t<T>>
                from_container(T & t, concepts::SizedIterable*, concepts::IteratorRange*)
                {
                    return {begin(t), end(t), size(t)};
                }

                template<typename T>
                static RANGES_RELAXED_CONSTEXPR range<range_iterator_t<T>, range_sentinel_t<T>>
                from_container(T & t, concepts::SizedIterable*, concepts::SizedIteratorRange*)
                {
                    RANGES_ASSERT(size(t) == size(begin(t), end(t)));
                    return {begin(t), end(t)};
                }

                /// If it's a range already, pass it though.
                template<typename T>
                static RANGES_RELAXED_CONSTEXPR T from_iterable(T && t, concepts::Range*)
                {
                    return std::forward<T>(t);
                }


                template<typename T>
                static void from_iterable_static_assert() {
                    static_assert(std::is_lvalue_reference<T>::value,
                                  "Cannot get a view of a temporary container");
                }

                /// If it is container-like, turn it into an range, being careful
                /// to preserve the Sized-ness of the iterable.
                template<typename T,
                    CONCEPT_REQUIRES_(!Range<T>()),
                    typename I = range_iterator_t<T>,
                    typename S = range_sentinel_t<T>,
                    typename SIC = sized_iterable_concept<T>,
                    typename SIRC = sized_iterator_range_concept<I, S>,
                    typename Assert = decltype(from_iterable_static_assert<T>())>
                static RANGES_RELAXED_CONSTEXPR auto from_iterable(T && t, concepts::Iterable*) ->
                    decltype(all_fn::from_container(t, SIC(), SIRC()))
                {
                    return all_fn::from_container(t, SIC(), SIRC());
                }

                // TODO handle char const * by turning it into a delimited range?

            public:
                template<typename T,
                    CONCEPT_REQUIRES_(Iterable<T>())>
                RANGES_RELAXED_CONSTEXPR auto operator()(T && t) const ->
                    decltype(all_fn::from_iterable(std::forward<T>(t), range_concept<T>()))
                {
                    return all_fn::from_iterable(std::forward<T>(t), range_concept<T>());
                }

                template<typename T,
                    CONCEPT_REQUIRES_(Iterable<T &>())>
                RANGES_RELAXED_CONSTEXPR
                ranges::reference_wrapper<T> operator()(std::reference_wrapper<T> ref) const
                {
                    return ranges::ref(ref.get());
                }
            };

            /// \relates all_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& all = static_const<all_fn>::value;
            }

            template<typename Rng>
            using all_t =
                meta::eval<std::decay<decltype(all(std::declval<Rng>()))>>;
        }
        /// @}
    }
}

#endif
