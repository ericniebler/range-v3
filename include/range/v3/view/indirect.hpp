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

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable>
        struct indirect_iterable_view
          : range_adaptor<indirect_iterable_view<InputIterable>, InputIterable>
        {
        private:
            friend range_core_access;
            using base_cursor_t = base_cursor_t<indirect_iterable_view>;
            struct adaptor : adaptor_defaults
            {
                auto current(base_cursor_t const &pos) const ->
                    decltype(*pos.current())
                {
                    return *pos.current();
                }
            };
            adaptor get_adaptor(begin_end_tag) const
            {
                return {};
            }
        public:
            explicit indirect_iterable_view(InputIterable && rng)
              : range_adaptor_t<indirect_iterable_view>(std::forward<InputIterable>(rng))
            {}
            CONCEPT_REQUIRES(SizedIterable<InputIterable>())
            range_size_t<InputIterable> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct indirecter : bindable<indirecter>, pipeable<indirecter>
            {
                template<typename InputIterable>
                static indirect_iterable_view<InputIterable>
                invoke(indirecter, InputIterable && rng)
                {
                    CONCEPT_ASSERT(ranges::Range<InputIterable>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                    return indirect_iterable_view<InputIterable>{std::forward<InputIterable>(rng)};
                }
            };

            RANGES_CONSTEXPR indirecter indirect{};
        }
    }
}

#endif
