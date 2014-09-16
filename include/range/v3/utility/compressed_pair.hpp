// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_COMPRESSED_PAIR_HPP
#define RANGES_V3_UTILITY_COMPRESSED_PAIR_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/box.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T, typename Enable = void>
            struct first_base
            {
                T first;
                first_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit first_base(U && u)
                  : first((U &&) u)
                {}
            };

            template<typename T>
            struct first_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>
            {
                static T first; // Can there be a data race "mutating" an empty object?
                first_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit first_base(U &&)
                {}
            };

            template<typename T>
            T first_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>::first{};

            template<typename T, typename Enable = void>
            struct second_base
            {
                T second;
                second_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit second_base(U && u)
                  : second((U &&) u)
                {}
            };

            template<typename T>
            struct second_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>
            {
                static T second;
                second_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit second_base(U && u)
                {}
            };

            template<typename T>
            T second_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>::second{};
        }

        template<typename First, typename Second>
        struct compressed_pair
          : private detail::first_base<First>
          , private detail::second_base<Second>
        {
            using detail::first_base<First>::first;
            using detail::second_base<Second>::second;

            compressed_pair() = default;

            constexpr compressed_pair(First f, Second s)
              : detail::first_base<First>{(First &&) f}
              , detail::second_base<Second>{(Second &&) s}
            {}

            template<typename F, typename S,
                typename std::enable_if<std::is_constructible<First, F &&>::value &&
                                        std::is_constructible<Second, S &&>::value, int>::type = 0>
            constexpr compressed_pair(F && f, S && s)
              : detail::first_base<First>{(F &&) f}
              , detail::second_base<Second>{(S &&) s}
            {}

            template<typename F, typename S,
                typename std::enable_if<std::is_constructible<F, First const &>::value &&
                                        std::is_constructible<S, Second const &>::value, int>::type = 0>
            constexpr operator std::pair<F, S> () const
            {
                return std::pair<F, S>{first, second};
            }
        };

        RANGES_CONSTEXPR struct make_compressed_pair_fn
        {
            template<typename First, typename Second>
            constexpr auto operator()(First && f, Second && s) const ->
                compressed_pair<First, Second>
            {
                return {detail::forward<First>(f), detail::forward<Second>(s)};
            }
        } make_compressed_pair {};

        //namespace
        //{
        //    inline void test_compressed_pair()
        //    {
        //        constexpr int i = compressed_pair<int, int>{4,5}.first;
        //        compressed_pair<int, int> p{4,5};
        //        int & j = p.first;
        //        compressed_pair<int, int> const pc{4,5};
        //        int const & k = pc.first;
        //        int & l = make_compressed_pair(j, 42).first;
        //        static_assert(std::is_same<decltype((make_compressed_pair(j, 42).second)), int>::value, "");
        //    }
        //}
    }
}

#endif
