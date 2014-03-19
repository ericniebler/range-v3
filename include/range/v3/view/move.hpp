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

#ifndef RANGES_V3_VIEW_MOVE_HPP
#define RANGES_V3_VIEW_MOVE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T> T && rref(T &, int);
            template<typename T> T rref(T, long);

            struct move_adaptor : adaptor_defaults
            {
            private:
                using adaptor_defaults::prev;
            public:
                using single_pass = std::true_type;
                template<typename Cursor>
                auto current(Cursor const &pos) const ->
                    decltype(detail::rref(pos.current(), 1))
                {
                    return std::move(pos.current());
                }
            };
        }

        template<typename InputIterable>
        struct move_iterable_view
          : range_adaptor<move_iterable_view<InputIterable>, InputIterable>
        {
        private:
            friend range_core_access;
            detail::move_adaptor get_adaptor(begin_end_tag) const
            {
                return {};
            }
        public:
            move_iterable_view(InputIterable &&rng)
              : range_adaptor_t<move_iterable_view>(std::forward<InputIterable>(rng))
            {}
            CONCEPT_REQUIRES(SizedIterable<InputIterable>())
            range_size_t<InputIterable> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct mover : bindable<mover>, pipeable<mover>
            {
                template<typename InputIterable>
                static move_iterable_view<InputIterable>
                invoke(mover, InputIterable && rng)
                {
                    CONCEPT_ASSERT(ranges::Range<InputIterable>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                    return move_iterable_view<InputIterable>{std::forward<InputIterable>(rng)};
                }
            };

            RANGES_CONSTEXPR mover move {};
        }
    }
}

#endif
