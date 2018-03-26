/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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

#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        struct empty_view
          : view_facade<empty_view<T>, (cardinality)0>
        {
        private:
            friend range_access;
            struct cursor
            {
                [[noreturn]] T const & read() const
                {
                    RANGES_ENSURE(false);
                }
                constexpr bool equal(default_sentinel) const
                {
                    return true;
                }
                constexpr bool equal(cursor const &) const
                {
                    return true;
                }
                [[noreturn]] void next()
                {
                    RANGES_ENSURE(false);
                }
                [[noreturn]] void prev()
                {
                    RANGES_ENSURE(false);
                }
                void advance(std::ptrdiff_t n)
                {
                    RANGES_EXPECT(n == 0);
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
            constexpr std::size_t size() const
            {
                return 0u;
            }
            constexpr T const *data() const
            {
                return nullptr;
            }
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

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::empty_view)

#endif
