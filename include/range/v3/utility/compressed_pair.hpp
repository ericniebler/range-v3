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
            struct first_tag {};
            struct second_tag {};
        }

        template<typename First, typename Second>
        class compressed_pair : box<First, detail::first_tag>
                              , box<Second, detail::second_tag>
        {
        public:
            compressed_pair() = default;

            template<typename F, typename S,
                typename std::enable_if<std::is_constructible<First, F &&>::value &&
                                        std::is_constructible<Second, S &&>::value, int>::type = 0>
            constexpr compressed_pair(F && f, S && s)
              : box<First, detail::first_tag>(detail::forward<F>(f)),
                box<Second, detail::second_tag>(detail::forward<S>(s))
            {}

            auto first() & -> First &
            {
                return ranges::get<detail::first_tag>(*this);
            }
            constexpr auto first() const & -> First const &
            {
                return ranges::get<detail::first_tag>(*this);
            }
            constexpr auto first() const && -> First &&
            {
                return ranges::get<detail::first_tag>(const_cast<compressed_pair &&>(*this));
            }

            auto second() & -> Second &
            {
                return ranges::get<detail::second_tag>(*this);
            }
            constexpr auto second() const & -> Second const &
            {
                return ranges::get<detail::second_tag>(*this);
            }
            constexpr auto second() const && -> Second &&
            {
                return ranges::get<detail::second_tag>(const_cast<compressed_pair &&>(*this));
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

