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

            struct move_adaptor : default_adaptor
            {
            private:
                using default_adaptor::prev;
            public:
                using single_pass = std::true_type;
                template<typename Cur>
                auto current(Cur const &pos) const ->
                    decltype(detail::rref(pos.current(), 1))
                {
                    return std::move(pos.current());
                }
            };
        }

        template<typename Rng>
        struct move_view
          : range_adaptor<move_view<Rng>, Rng>
        {
        private:
            friend range_core_access;
            detail::move_adaptor begin_adaptor() const
            {
                return {};
            }
            detail::move_adaptor end_adaptor() const
            {
                return {};
            }
        public:
            move_view(Rng &&rng)
              : range_adaptor_t<move_view>(std::forward<Rng>(rng))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_t<Rng> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct move_fn : bindable<move_fn>, pipeable<move_fn>
            {
                template<typename Rng>
                static move_view<Rng>
                invoke(move_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(InputBoundedRange<Rng>());
                    return move_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR move_fn move {};
        }
    }
}

#endif
