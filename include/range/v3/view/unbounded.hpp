//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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
        template<typename InputIterator>
        struct unbounded_iterable_view
          : private detail::is_infinite<true>
          , private range_base
        {
        private:
            InputIterator it_;
        public:
            constexpr explicit unbounded_iterable_view(InputIterator it)
              : it_(detail::move(it))
            {}
            constexpr InputIterator begin() const
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
            struct unbounded_maker : bindable<unbounded_maker>
            {
                template<typename InputIterator>
                static constexpr unbounded_iterable_view<InputIterator>
                invoke(unbounded_maker, InputIterator it)
                {
                    CONCEPT_ASSERT(ranges::InputIterator<InputIterator>());
                    return unbounded_iterable_view<InputIterator>{detail::move(it)};
                }
            };

            RANGES_CONSTEXPR unbounded_maker unbounded {};
        }
    }
}

#endif
