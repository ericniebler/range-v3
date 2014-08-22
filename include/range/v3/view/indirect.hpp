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
        namespace detail
        {
            struct indirect_adaptor : default_adaptor
            {
                template<typename Cur>
                auto current(Cur const &pos) const -> decltype(*pos.current())
                {
                    return *pos.current();
                }
            };
        }

        template<typename Rng>
        struct indirect_view
          : range_adaptor<indirect_view<Rng>, Rng>
        {
        private:
            friend range_core_access;
            detail::indirect_adaptor begin_adaptor() const
            {
                return {};
            }
            detail::indirect_adaptor end_adaptor() const
            {
                return{};
            }
        public:
            explicit indirect_view(Rng && rng)
              : range_adaptor_t<indirect_view>(std::forward<Rng>(rng))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct indirect_fn : bindable<indirect_fn>, pipeable<indirect_fn>
            {
                template<typename Rng>
                static indirect_view<Rng>
                invoke(indirect_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    return indirect_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR indirect_fn indirect{};
        }
    }
}

#endif
