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
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct const_range_view
        {
        private:
            Rng rng_;
        public:
            using iterator = decltype(ranges::cbegin(std::declval<Rng const &>()));
            using const_iterator = iterator;

            explicit const_range_view(Rng && rng)
              : rng_(rng)
            {}
            iterator begin() const
            {
                return ranges::cbegin(rng_);
            }
            iterator end() const
            {
                return ranges::cend(rng_);
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
        };

        namespace view
        {
            struct conster : bindable<conster>, pipeable<conster>
            {
                template<typename Rng>
                static const_range_view<Rng> invoke(conster, Rng && rng)
                {
                    return const_range_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR conster const_ {};
        }
    }
}

#endif
