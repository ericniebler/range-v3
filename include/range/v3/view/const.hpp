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
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T>
            T const & cref(T const &);

            struct const_adaptor : adaptor_defaults
            {
                template<typename Cursor>
                auto current(Cursor const &pos) const ->
                    decltype(true ? pos.current() : detail::cref(pos.current()))
                {
                    return pos.current();
                }
            };
        }

        template<typename Iterable>
        struct const_iterable_view
          : range_adaptor<const_iterable_view<Iterable>, Iterable>
        {
        private:
            friend range_core_access;
            detail::const_adaptor get_adaptor(begin_end_tag) const
            {
                return {};
            }
        public:
            explicit const_iterable_view(Iterable && rng)
              : range_adaptor_t<const_iterable_view>(std::forward<Iterable>(rng))
            {}
            CONCEPT_REQUIRES(SizedIterable<Iterable>())
            range_size_t<Iterable> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct conster : bindable<conster>, pipeable<conster>
            {
                template<typename Iterable>
                static const_iterable_view<Iterable> invoke(conster, Iterable && rng)
                {
                    CONCEPT_ASSERT(ranges::Iterable<Iterable>());
                    return const_iterable_view<Iterable>{std::forward<Iterable>(rng)};
                }
            };

            RANGES_CONSTEXPR conster const_ {};
        }
    }
}

#endif
