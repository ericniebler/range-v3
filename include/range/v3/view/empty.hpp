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

#ifndef RANGES_V3_VIEW_EMPTY_HPP
#define RANGES_V3_VIEW_EMPTY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        struct empty_view
          : range_facade<empty_view<T>>
        {
        private:
            friend range_access;
            struct cursor
            {
                T const & current() const
                {
                    RANGES_ASSERT(false);
                    return *reinterpret_cast<T const *>(this);
                }
                constexpr bool done() const
                {
                    return true;
                }
                constexpr bool equal(cursor const &) const
                {
                    return true;
                }
                void next()
                {
                    RANGES_ASSERT(false);
                }
                void prev()
                {
                    RANGES_ASSERT(false);
                }
                void advance(std::ptrdiff_t n)
                {
                    RANGES_ASSERT(n == 0);
                }
                std::ptrdiff_t distance_to(cursor const &) const
                {
                    return 0;
                }
            };
            cursor begin_cursor() const
            {
                return {};
            }
            cursor end_cursor() const
            {
                return {};
            }
        public:
            empty_view() = default;
        };

        namespace view
        {
            template<typename T>
            empty_view<T> empty()
            {
                return {};
            }
        }
    }
}

#endif
