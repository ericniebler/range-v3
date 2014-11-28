// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_SINGLE_HPP
#define RANGES_V3_VIEW_SINGLE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Val>
        struct single_view
          : range_facade<single_view<Val>>
        {
        private:
            friend struct range_access;
            Val value_;
            struct cursor
            {
            private:
                Val value_;
                bool done_;
            public:
                using single_pass = std::true_type;
                cursor() = default;
                cursor(Val value)
                  : value_(std::move(value)), done_(false)
                {}
                bool done() const
                {
                    return done_;
                }
                void next()
                {
                    done_ = true;
                }
                Val current() const
                {
                    return value_;
                }
            };
            cursor begin_cursor() const
            {
                return {value_};
            }
        public:
            single_view() = default;
            constexpr explicit single_view(Val value)
              : value_(detail::move(value))
            {}
            constexpr std::size_t size() const
            {
                return 1;
            }
        };

        namespace view
        {
            struct single_fn
            {
                template<typename Val>
                single_view<Val> operator()(Val value) const
                {
                    return single_view<Val>{std::move(value)};
                }
            };

            constexpr single_fn single {};
        }
    }
}

#endif
