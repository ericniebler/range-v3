// Range v3 library
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

#ifndef RANGES_V3_VIEW_TAIL_HPP
#define RANGES_V3_VIEW_TAIL_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/size.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct tail_view
          : range_interface<tail_view<Rng>, is_infinite<Rng>::value>
        {
        private:
            using base_range_t = view::all_t<Rng>;
            base_range_t rng_;
        public:
            using iterator = range_iterator_t<base_range_t>;
            using sentinel = range_sentinel_t<base_range_t>;

            tail_view() = default;
            tail_view(Rng &&rng)
              : rng_(view::all(std::forward<Rng>(rng)))
            {
                CONCEPT_ASSERT(InputIterable<Rng>());
                RANGES_ASSERT(!ForwardIterable<Rng>() || !empty(rng_));
            }
            iterator begin() const
            {
                return next(ranges::begin(rng_));
            }
            sentinel end() const
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(SizedRange<base_range_t>())
            range_size_t<base_range_t> size() const
            {
                return ranges::size(rng_) - 1;
            }
            base_range_t & base()
            {
                return rng_;
            }
            base_range_t const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct tail_fn
            {
                template<typename Rng, CONCEPT_REQUIRES_(InputIterable<Rng>())>
                tail_view<Rng> operator()(Rng && rng) const
                {
                    return tail_view<Rng>{std::forward<Rng>(rng)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, CONCEPT_REQUIRES_(!InputIterable<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object on which view::tail is to operate must be a model of the "
                        "InputIterable concept.");
                }
            #endif
            };

            /// \sa `tail_fn`
            /// \ingroup group-views
            constexpr view<tail_fn> tail{};
        }
        /// @}
    }
}

#endif
