//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_UNBOUNDED_HPP
#define RANGES_V3_VIEW_UNBOUNDED_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I>
        struct unbounded_view
          : private detail::is_infinite<true>
          , private range_base
        {
        private:
            I it_;
        public:
            constexpr explicit unbounded_view(I it)
              : it_(detail::move(it))
            {}
            constexpr I begin() const
            {
                return it_;
            }
            constexpr unreachable end() const
            {
                return {};
            }
        };

        namespace view
        {
            struct unbounded_fn : bindable<unbounded_fn>
            {
                template<typename I>
                static constexpr unbounded_view<I>
                invoke(unbounded_fn, I it)
                {
                    CONCEPT_ASSERT(InputIterator<I>());
                    return unbounded_view<I>{detail::move(it)};
                }
            };

            RANGES_CONSTEXPR unbounded_fn unbounded {};
        }
    }
}

#endif
