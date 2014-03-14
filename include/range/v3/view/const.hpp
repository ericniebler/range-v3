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

#ifndef RANGES_V3_VIEW_CONST_HPP
#define RANGES_V3_VIEW_CONST_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Iterable>
        struct const_range_view : private range_base
        {
        private:
            Iterable rng_;
        public:
            using iterator = range_iterator_t<detail::as_cref_t<Iterable>>;
            using sentinel = range_sentinel_t<detail::as_cref_t<Iterable>>;

            explicit const_range_view(Iterable && rng)
              : rng_(std::forward<Iterable>(rng))
            {}
            iterator begin() const
            {
                return ranges::cbegin(rng_);
            }
            sentinel end() const
            {
                return ranges::cend(rng_);
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return !!*this;
            }
        };

        namespace view
        {
            struct conster : bindable<conster>, pipeable<conster>
            {
                template<typename Iterable>
                static const_range_view<Iterable> invoke(conster, Iterable && rng)
                {
                    CONCEPT_ASSERT(ranges::Iterable<Iterable>());
                    return const_range_view<Iterable>{std::forward<Iterable>(rng)};
                }
            };

            RANGES_CONSTEXPR conster const_ {};
        }
    }
}

#endif
