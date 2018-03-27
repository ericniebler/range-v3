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

#ifndef RANGES_V3_VIEW_SINGLE_HPP
#define RANGES_V3_VIEW_SINGLE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Val>
        struct single_view
          : view_facade<single_view<Val>, (cardinality)1>
        {
        private:
            friend struct ranges::range_access;
            semiregular_t<Val> value_;
            struct cursor
            {
            private:
                semiregular_t<Val> value_;
                bool done_;
            public:
                cursor() = default;
                explicit cursor(Val value)
                  : value_(std::move(value)), done_(false)
                {}
                Val read() const
                {
                    return value_;
                }
                bool equal(default_sentinel) const
                {
                    return done_;
                }
                bool equal(cursor const &that) const
                {
                    return done_ == that.done_;
                }
                void next()
                {
                    done_ = true;
                }
                void prev()
                {
                    done_ = false;
                }
                void advance(std::ptrdiff_t n)
                {
                    n += done_;
                    RANGES_EXPECT(n == 0 || n == 1);
                    done_ = n != 0;
                }
                std::ptrdiff_t distance_to(cursor const &that) const
                {
                    return that.done_ - done_;
                }
            };
            cursor begin_cursor() const
            {
                return cursor{value_};
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
                template<typename Val, CONCEPT_REQUIRES_(CopyConstructible<Val>())>
                single_view<Val> operator()(Val value) const
                {
                    return single_view<Val>{std::move(value)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Arg, typename Val = detail::decay_t<Arg>,
                    CONCEPT_REQUIRES_(!(CopyConstructible<Val>() && Constructible<Val, Arg>()))>
                void operator()(Arg &&) const
                {
                    CONCEPT_ASSERT_MSG(CopyConstructible<Val>(),
                        "The object passed to view::single must be a model of the CopyConstructible "
                        "concept; that is, it needs to be copy and move constructible, and destructible.");
                    CONCEPT_ASSERT_MSG(!CopyConstructible<Val>() || Constructible<Val, Arg>(),
                        "The object type passed to view::single must be initializable from the "
                        "actual argument expression.");
                }
            #endif
            };

            /// \relates single_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(single_fn, single)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::single_view)

#endif
