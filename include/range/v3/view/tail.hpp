/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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
#include <range/v3/detail/satisfy_boost_range.hpp>
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
        namespace detail
        {
            template<typename T>
            constexpr T prev_or_zero_(T n)
            {
                return n == 0 ? T(0) : T(n - 1);
            }
        }

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct tail_view
          : view_interface<
                tail_view<Rng>,
                (range_cardinality<Rng>::value >= 0)
                  ? detail::prev_or_zero_(range_cardinality<Rng>::value)
                  : range_cardinality<Rng>::value>
        {
        private:
            Rng rng_;
            using size_type_ = range_size_type_t<Rng>;
        public:
            using iterator = iterator_t<Rng>;
            using sentinel = sentinel_t<Rng>;

            tail_view() = default;
            tail_view(Rng rng)
              : rng_(static_cast<Rng&&>(rng))
            {
                CONCEPT_ASSERT(InputRange<Rng>());
            }
            iterator begin()
            {
                return next(ranges::begin(rng_), 1, ranges::end(rng_));
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            iterator begin() const
            {
                return next(ranges::begin(rng_), 1, ranges::end(rng_));
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
            constexpr size_type_ size() const
            {
                return range_cardinality<Rng>::value >= 0
                  ? detail::prev_or_zero_((size_type_)range_cardinality<Rng>::value)
                  : detail::prev_or_zero_(ranges::size(rng_));
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
                meta::if_c<range_cardinality<Rng>::value == 0, all_t<Rng>, tail_view<all_t<Rng>>>
                operator()(Rng && rng) const
                {
                    return all(static_cast<Rng&&>(rng));
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

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::tail_view)

#endif
