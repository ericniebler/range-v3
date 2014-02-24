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
        template<typename InputRange>
        struct indirect_range_view
          : range_adaptor<indirect_range_view<InputRange>, InputRange>
        {
        private:
            friend struct range_core_access;
            using base_t = range_adaptor<indirect_range_view, InputRange>;
            using impl_base_t = range_core_access::const_impl_t<base_t>;

            struct impl : impl_base_t
            {
                impl() = default;
                impl(impl_base_t base)
                  : impl_base_t(std::move(base))
                {}
                auto current() const -> decltype(*this->base().current())
                {
                    return *this->base().current();
                }
            };
            impl begin_impl() const
            {
                return {this->adaptor().begin_impl()};
            }
            impl end_impl() const
            {
                return {this->adaptor().end_impl()};
            }
        public:
            explicit indirect_range_view(InputRange && rng)
              : base_t(std::forward<InputRange>(rng))
            {}
        };

        namespace view
        {
            struct indirecter : bindable<indirecter>, pipeable<indirecter>
            {
                template<typename InputRange>
                static indirect_range_view<InputRange>
                invoke(indirecter, InputRange && rng)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                    return indirect_range_view<InputRange>{std::forward<InputRange>(rng)};
                }
            };

            RANGES_CONSTEXPR indirecter indirect{};
        }
    }
}

#endif
