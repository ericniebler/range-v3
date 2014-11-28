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

#ifndef RANGES_V3_VIEW_REPEAT_HPP
#define RANGES_V3_VIEW_REPEAT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/pipeable.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Ordinarily, a view shouldn't contain its elements. This is so that copying
        // and assigning ranges is O(1), and also so that in the event of element
        // mutation, all the copies of the range see the mutation the same way. The
        // repeated_view *does* own its lone element, though. This is OK because:
        //  - O(N) copying is fine when N==1 as it is in this case, and
        //  - The element is immutable, so there is no potential for incorrect
        //    semantics.
        template<typename Val>
        struct repeated_view
          : range_facade<repeated_view<Val>, true>
        {
        private:
            Val value_;
            friend range_access;

            struct cursor
            {
            private:
                Val value_;
            public:
                cursor() = default;
                cursor(Val value)
                  : value_(value)
                {}
                Val current() const
                {
                    return value_;
                }
                constexpr bool done() const
                {
                    return false;
                }
                void next() const
                {}
            };
            cursor begin_cursor() const
            {
                return {value_};
            }
        public:
            repeated_view() = default;
            constexpr explicit repeated_view(Val value)
              : value_(detail::move(value))
            {}
        };

        namespace view
        {
            struct repeat_fn : pipeable<repeat_fn>
            {
                template<typename Val>
                repeated_view<Val> operator()(Val value) const
                {
                    CONCEPT_ASSERT(SemiRegular<Val>());
                    return repeated_view<Val>{std::move(value)};
                }
            };

            constexpr repeat_fn repeat{};
        }
    }
}

#endif
