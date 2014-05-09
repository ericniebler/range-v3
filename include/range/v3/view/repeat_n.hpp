// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_REPEAT_N_HPP
#define RANGES_V3_VIEW_REPEAT_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        // BUGBUG a view shouldn't contain its value, right?
        template<typename Val>
        struct repeated_n_view
          : range_facade<repeated_n_view<Val>, true>
        {
        private:
            friend range_core_access;
            Val value_;
            std::size_t n_;

            struct cursor
            {
            private:
                Val value_;
                std::size_t n_;
            public:
                using single_pass = std::true_type;
                cursor() = default;
                cursor(Val value, std::size_t n)
                  : value_(std::move(value)), n_(n)
                {}
                Val current() const
                {
                    return value_;
                }
                constexpr bool done() const
                {
                    return 0 == n_;
                }
                void next()
                {
                    RANGES_ASSERT(0 != n_);
                    --n_;
                }
            };
            cursor begin_cursor() const
            {
                return {value_, n_};
            }
        public:
            repeated_n_view() = default;
            constexpr repeated_n_view(Val value, std::size_t n)
              : value_(detail::move(value)), n_(n)
            {}
            constexpr std::size_t size() const
            {
                return n_;
            }
        };

        namespace view
        {
            struct repeat_n_fn : bindable<repeat_n_fn>, pipeable<repeat_n_fn>
            {
                template<typename Val>
                static repeated_n_view<Val> invoke(repeat_n_fn, Val value, std::size_t n)
                {
                    CONCEPT_ASSERT(SemiRegular<Val>());
                    return repeated_n_view<Val>{std::move(value), n};
                }
            };

            RANGES_CONSTEXPR repeat_n_fn repeat_n{};
        }
    }
}

#endif
