/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
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

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{

        // Ordinarily, a view shouldn't contain its elements. This is so that copying
        // and assigning ranges is O(1), and also so that in the event of element
        // mutation, all the copies of the range see the mutation the same way. The
        // repeat_view *does* own its lone element, though. This is OK because:
        //  - O(N) copying is fine when N==1 as it is in this case, and
        //  - The element is immutable, so there is no potential for incorrect
        //    semantics.
        template<typename Val>
        struct repeat_view
          : view_facade<repeat_view<Val>, infinite>
        {
        private:
            semiregular_t<Val> value_;
            friend range_access;

            struct cursor
            {
            private:
                Val const *value_;
            public:
                cursor() = default;
                explicit cursor(Val const &value)
                  : value_(std::addressof(value))
                {}
                Val const &read() const
                {
                    return *value_;
                }
                constexpr bool equal(default_sentinel) const
                {
                    return false;
                }
                bool equal(cursor const &) const
                {
                    return true;
                }
                void next()
                {}
                void prev()
                {}
                void advance(std::ptrdiff_t)
                {}
                std::ptrdiff_t distance_to(cursor const &) const
                {
                    return 0;
                }
            };
            cursor begin_cursor() const
            {
                return cursor{value_};
            }
        public:
            repeat_view() = default;
            constexpr explicit repeat_view(Val value)
              : value_(detail::move(value))
            {}
        };

        namespace view
        {
            struct repeat_fn
            {
                template<typename Val,
                    CONCEPT_REQUIRES_(CopyConstructible<Val>() && std::is_object<Val>())>
                repeat_view<Val> operator()(Val value) const
                {
                    return repeat_view<Val>{std::move(value)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Val,
                    CONCEPT_REQUIRES_(!(CopyConstructible<Val>() && std::is_object<Val>()))>
                void operator()(Val) const
                {
                    CONCEPT_ASSERT_MSG(std::is_object<Val>(),
                        "The value passed to view::repeat must be an object.");
                    CONCEPT_ASSERT_MSG(CopyConstructible<Val>(),
                        "The value passed to view::repeat must be CopyConstructible.");
                }
            #endif
            };

            /// \relates repeat_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(repeat_fn, repeat)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::repeat_view)

#endif
