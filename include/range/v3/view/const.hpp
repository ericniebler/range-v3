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

            struct const_adaptor : default_adaptor
            {
                template<typename Cursor>
                auto current(Cursor const &pos) const ->
                    decltype(true ? pos.current() : detail::cref(pos.current()))
                {
                    return pos.current();
                }
            };
        }

        template<typename Rng>
        struct const_view
          : range_adaptor<const_view<Rng>, Rng>
        {
        private:
            friend range_core_access;
            detail::const_adaptor begin_adaptor() const
            {
                return {};
            }
            detail::const_adaptor end_adaptor() const
            {
                return{};
            }
        public:
            explicit const_view(Rng && rng)
              : range_adaptor_t<const_view>(std::forward<Rng>(rng))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct conster : bindable<conster>, pipeable<conster>
            {
                template<typename Rng>
                static const_view<Rng> invoke(conster, Rng && rng)
                {
                    CONCEPT_ASSERT(ranges::Iterable<Rng>());
                    return const_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR conster const_ {};
        }
    }
}

#endif
