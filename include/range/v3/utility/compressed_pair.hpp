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

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Element, int I, typename Enable = void>
            struct box
            {
            private:
                Element value_;
            public:
                box() = default;

                template<typename E,
                    typename = typename std::enable_if<std::is_constructible<Element, E &&>::value>::type>
                constexpr explicit box(E && e)
                  : value_(detail::forward<E>(e))
                {}

                auto get() & -> Element &
                {
                    return value_;
                }
                constexpr auto get() const & -> Element const &
                {
                    return value_;
                }
                constexpr auto get() const && -> Element &&
                {
                    return const_cast<box &&>(*this).value_;
                }
            };

            template<typename Element, int I>
            struct box<Element, I, typename std::enable_if<std::is_empty<Element>::value>::type>
              : private Element
            {
                box() = default;

                template<typename E,
                    typename = typename std::enable_if<std::is_constructible<Element, E &&>::value>::type>
                constexpr explicit box(E && e)
                  : Element(detail::forward<E>(e))
                {}

                auto get() & -> Element &
                {
                    return *this;
                }
                constexpr auto get() const & -> Element const &
                {
                    return *this;
                }
                constexpr auto get() const && -> Element &&
                {
                    return const_cast<box &&>(*this);
                }
            };
        }

        template<typename First, typename Second>
        class compressed_pair : detail::box<First, 0>, detail::box<Second, 1>
        {
        public:
            compressed_pair() = default;

            template<typename F, typename S,
                typename = typename std::enable_if<std::is_constructible<First, F &&>::value>::type,
                typename = typename std::enable_if<std::is_constructible<Second, S &&>::value>::type>
            constexpr compressed_pair(F && f, S && s)
              : detail::box<First, 0>(detail::forward<F>(f)),
                detail::box<Second, 1>(detail::forward<S>(s))
            {}

            auto first() & -> First &
            {
                return detail::box<First, 0>::get();
            }
            constexpr auto first() const & -> First const &
            {
                return detail::box<First, 0>::get();
            }
            constexpr auto first() const && -> First &&
            {
                return const_cast<compressed_pair &&>(*this).detail::box<First, 0>::get();
            }

            auto second() & -> Second &
            {
                return detail::box<Second, 1>::get();
            }
            constexpr auto second() const & -> Second const &
            {
                return detail::box<Second, 1>::get();
            }
            constexpr auto second() const && -> Second &&
            {
                return const_cast<compressed_pair &&>(*this).detail::box<Second, 1>::get();
            }
        };

        RANGES_CONSTEXPR struct compressed_pair_maker
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
        //        constexpr int i = compressed_pair<int, int>{4,5}.first();
        //        compressed_pair<int, int> p{4,5};
        //        int & j = p.first();
        //        compressed_pair<int, int> const pc{4,5};
        //        int const & k = pc.first();
        //        int & l = make_compressed_pair(j, 42).first();
        //        static_assert(std::is_same<decltype(make_compressed_pair(j, 42).second()), int &&>::value, "");
        //    }
        //}
    }
}

#endif

