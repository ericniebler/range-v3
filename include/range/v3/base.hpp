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
        namespace detail
        {
            template<class V>
            RANGES_CXX14_CONSTEXPR auto base_loop(V&& v, std::integral_constant<int, 0>)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                std::forward<V>(v)
            )
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

            template<typename... Ts> struct make_void { typedef void type;};
            template<typename... Ts> using void_t = typename make_void<Ts...>::type;

            template<class T>
            using remove_reference_t = typename std::remove_reference<T>::type;

            template<class From, class To, int I = 0, bool is_same = std::is_same< remove_reference_t<From>, remove_reference_t<To> >::value>
            struct base_distance;

            template<class From, class To, int I>
            struct base_distance<From, To, I, true>{
                static const constexpr int value = I;
            };

            template<class From, class To, int I>
            struct base_distance<From, To, I, false>{
                template< class, class = void_t<> >
                struct has_base {
                    static const constexpr int value = -1;
                };
                template<class T>
                struct has_base<T, void_t<decltype(std::declval<T>().base())>> {
                    static const constexpr int value = base_distance<decltype(std::declval<From>().base()), To, I+1>::value;
                };

                static const constexpr int value = has_base<From>::value;
            };

            template<class I, class BaseRange>
            struct iterator_or_sentinel_base_distance{
                static const constexpr int iterator_n = detail::base_distance<I, iterator_t<BaseRange>>::value;
                static const constexpr int value = iterator_n >= 0 ? iterator_n : detail::base_distance<I, sentinel_t<BaseRange>>::value;
                static_assert(value >= 0 , "");
            };
        }

        // iterators

        // TODO: static_assert(N > 0)
        template<int N = 1, typename I,
            CONCEPT_REQUIRES_(Iterator<I>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::base_loop<N>(std::forward<I>(iter))
        )

        template<typename BaseIterator, typename I,
            CONCEPT_REQUIRES_(Iterator<BaseIterator>() && Iterator<I>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            base<
                detail::base_distance<I, BaseIterator>::value
            >(std::forward<I>(iter))
        )

        template<typename BaseRange, typename I,
            CONCEPT_REQUIRES_(Iterator<I>() && Range<BaseRange>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            base<
                detail::iterator_or_sentinel_base_distance<I, BaseRange>::value
            >(std::forward<I>(iter))
        )
        /*template<typename BaseRange, typename I,
            CONCEPT_REQUIRES_(Iterator<I>() && Range<BaseRange>())>
        RANGES_CXX14_CONSTEXPR auto base(I&& iter, BaseRange&&)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            base<iterator_t<BaseRange>>(std::forward<I>(iter))
        )*/

        // ranges

        template<int N = 1, typename R,
            CONCEPT_REQUIRES_(Range<R>())>
        RANGES_CXX14_CONSTEXPR auto base(R&& range)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::base_loop<N>(std::forward<R>(range))
        )
        template<typename BaseRange, typename R,
            CONCEPT_REQUIRES_(Range<R>() && Range<BaseRange>())>
        RANGES_CXX14_CONSTEXPR auto base(R&& range/*, std::false_type*/)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            base<
                detail::base_distance<R, view::all_t<BaseRange>>::value
            >(view::all(std::forward<R>(range)))
        )
    }
}

#endif //RANGE_V3_BASE_HPP
