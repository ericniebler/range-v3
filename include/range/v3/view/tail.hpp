/// \file
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
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/static_const.hpp>
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
          : view_interface<
                tail_view<Rng>,
                (range_cardinality<Rng>::value > 0) ?
                    (cardinality)(range_cardinality<Rng>::value - 1) :
                    range_cardinality<Rng>::value>
        {
        private:
            Rng rng_;
        public:
            using iterator = range_iterator_t<Rng>;
            using sentinel = range_sentinel_t<Rng>;

            tail_view() = default;
            tail_view(Rng rng)
              : rng_(std::forward<Rng>(rng))
            {
                CONCEPT_ASSERT(InputRange<Rng>());
                RANGES_ASSERT(!ForwardRange<Rng>() || !empty(rng_));
            }
            iterator begin()
            {
                return next(ranges::begin(rng_));
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            iterator begin() const
            {
                return next(ranges::begin(rng_));
            }
            sentinel end()
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            sentinel end() const
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(SizedView<Rng>())
            constexpr range_size_t<Rng> size() const
            {
                return range_cardinality<Rng>::value > 0 ?
                    (range_size_t<Rng>)range_cardinality<Rng>::value - 1 :
                    ranges::size(rng_) - 1;
            }
            Rng & base()
            {
                return rng_;
            }
            Rng const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct tail_fn
            {
                template<typename Rng, CONCEPT_REQUIRES_(InputRange<Rng>())>
                tail_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    static_assert(range_cardinality<Rng>::value != 0,
                        "Can't take the tail of an empty range.");
                    return tail_view<all_t<Rng>>{all(std::forward<Rng>(rng))};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, CONCEPT_REQUIRES_(!InputRange<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::tail is to operate must be a model of the "
                        "InputRange concept.");
                }
            #endif
            };

            /// \relates tail_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<tail_fn>, tail)
        }
        /// @}
    }
}

#endif
