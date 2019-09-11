/// \file
// Range v3 library
//
//  Copyright Filip Matzner 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_EXPERIMENTAL_VIEW_SHARED_HPP
#define RANGES_V3_EXPERIMENTAL_VIEW_SHARED_HPP

#include <memory>
#include <type_traits>

#include <meta/meta.hpp>

#include <range/v3/functional/pipeable.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    namespace experimental
    {
        template<typename Rng>
        struct shared_view
          : view_interface<shared_view<Rng>, range_cardinality<Rng>::value>
        {
        private:
            // shared storage
            std::shared_ptr<Rng> rng_ptr_;

        public:
            shared_view() = default;

            // construct from a range rvalue
            explicit shared_view(Rng rng)
              : rng_ptr_{std::make_shared<Rng>(std::move(rng))}
            {}

            // use the stored range's begin and end
            iterator_t<Rng> begin() const
            {
                return ranges::begin(*rng_ptr_);
            }
            sentinel_t<Rng> end() const
            {
                return ranges::end(*rng_ptr_);
            }

            CPP_member
            auto CPP_fun(size)()(const requires sized_range<Rng>)
            {
                return ranges::size(*rng_ptr_);
            }
        };

        /// \relates all
        /// \addtogroup group-views
        /// @{
        namespace views
        {
            struct shared_fn : pipeable_base
            {
                template<typename Rng>
                auto operator()(Rng && rng) const                       //
                    -> CPP_ret(shared_view<detail::decay_t<Rng>>)(      //
                        requires range<Rng> && (!viewable_range<Rng>)&& //
                        constructible_from<detail::decay_t<Rng>, Rng>)
                {
                    return shared_view<detail::decay_t<Rng>>{static_cast<Rng &&>(rng)};
                }
            };

            /// \relates shared_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(shared_fn, shared)
        } // namespace views
        /// @}
    } // namespace experimental
} // namespace ranges

#endif // include guard
