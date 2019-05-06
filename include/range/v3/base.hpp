/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_BASE_HPP
#define RANGE_V3_BASE_HPP

#include <range/v3/range_concepts.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace details
        {
            template<class V>
            RANGES_CXX14_CONSTEXPR auto base_loop(V&& v, std::integral_constant<int, 1>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                v.base()
            )
            template<int N, class V>
            RANGES_CXX14_CONSTEXPR auto base_loop(V&& v, std::integral_constant<int, N>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                base_loop( v.base(), std::integral_constant<int, N-1>{})
            )
            template<int N, class V>
            RANGES_CXX14_CONSTEXPR auto base_loop(V&& v)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                base_loop(std::forward<V>(v), std::integral_constant<int, N>{})
            )

            template<typename Target, bool found>
            struct loop_to_t;

            template<typename Target, class V>
            RANGES_CXX14_CONSTEXPR auto loop_to(V&& v)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                loop_to_t<
                    Target,
                    std::is_same< detail::decay_t<V>, detail::decay_t<Target> >::value
                >{}(std::forward<V>(v))
            )

            template<typename Target>
            struct loop_to_t<Target, true>
            {
                template<class V>
                RANGES_CXX14_CONSTEXPR auto operator()(V&& v)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    std::forward<V>(v)
                )
            };

            template<typename Target>
            struct loop_to_t<Target, false>
            {
                template<class V>
                RANGES_CXX14_CONSTEXPR auto operator()(V&& v)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    loop_to<Target>(v.base())
                )
            };
        }

        // for iterators
        template<typename I,
            CONCEPT_REQUIRES_(Iterator<I>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            iter.base()
        )

        // TODO: static_assert(N >= 1)
        template<int N, typename I,
            CONCEPT_REQUIRES_(Iterator<I>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            details::base_loop<N>(std::forward<I>(iter))
        )
        template<typename BaseIterator, typename I,
            CONCEPT_REQUIRES_(Iterator<I>() && Iterator<BaseIterator>())>
        RANGES_CXX14_CONSTEXPR BaseIterator base(I&& iter)
        {
            return details::loop_to<BaseIterator>(std::forward<I>(iter));
        }
        template<typename BaseRange, typename I,
            CONCEPT_REQUIRES_(Iterator<I>() && Range<BaseRange>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            base<iterator_t<BaseRange>>(std::forward<I>(iter))
        )
        template<typename BaseRange, typename I,
            CONCEPT_REQUIRES_(Iterator<I>() && Range<BaseRange>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter, BaseRange&&)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            base<iterator_t<BaseRange>>(std::forward<I>(iter))
        )

        // for ranges
        template<typename R,
            CONCEPT_REQUIRES_(Range<R>())>
        RANGES_CXX14_CONSTEXPR auto base(R&& range)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            range.base()
        )
        template<int N, typename R,
            CONCEPT_REQUIRES_(Range<R>())>
        RANGES_CXX14_CONSTEXPR auto base(R&& range)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            details::base_loop<N>(std::forward<R>(range))
        )
        template<typename BaseRange, typename R,
            CONCEPT_REQUIRES_(Range<R>() && Range<BaseRange>())>
        RANGES_CXX14_CONSTEXPR auto base(R&& range)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            details::loop_to<view::all_t<BaseRange>>(view::all(std::forward<R>(range)))
        )
    }
}

#endif //RANGE_V3_BASE_HPP
