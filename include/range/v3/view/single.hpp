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
        /// \addtogroup group-views
        /// @{
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
                cursor() = default;
                cursor(Val value)
                  : value_(std::move(value)), done_(false)
                {}
                Val current() const
                {
                    return value_;
                }
                bool done() const
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
                    RANGES_ASSERT(n == 0 || n == 1);
                    done_ = n != 0;
                }
                std::ptrdiff_t distance_to(cursor const &that) const
                {
                    return that.done_ - done_;
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
                template<typename Val, CONCEPT_REQUIRES_(SemiRegular<Val>())>
                single_view<Val> operator()(Val value) const
                {
                    return single_view<Val>{std::move(value)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Val, CONCEPT_REQUIRES_(!SemiRegular<Val>())>
                void operator()(Val &&) const
                {
                    CONCEPT_ASSERT_MSG(SemiRegular<Val>(),
                        "The object passed to view::single must be a model of the SemiRegular "
                        "concept; that is, it needs to be default constructible, copy and move "
                        " constructible, and destructible.");
                }
            #endif
            };

            /// \sa `single_fn`
            /// \ingroup group-views
            constexpr single_fn single{};
        }
        /// @}
    }
}

#endif
