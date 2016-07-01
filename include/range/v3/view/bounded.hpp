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
#ifndef RANGES_V3_VIEW_BOUNDED_HPP
#define RANGES_V3_VIEW_BOUNDED_HPP

#include <type_traits>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
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
        struct bounded_view
          : view_interface<bounded_view<Rng>, range_cardinality<Rng>::value>
        {
        private:
            using RA_and_Sized =
                meta::strict_and<RandomAccessRange<Rng>, SizedRange<Rng>>;
            using base_iterator_t = range_iterator_t<Rng>;
            using base_sentinel_t = range_sentinel_t<Rng>;
            Rng rng_;

            base_sentinel_t end_(std::false_type)
            {
                return ranges::end(rng_);
            }
            base_iterator_t end_(std::true_type)
            {
                return ranges::begin(rng_) + ranges::distance(rng_);
            }
        public:
            using iterator = meta::if_<RA_and_Sized,
                base_iterator_t,
                common_iterator<base_iterator_t, base_sentinel_t>>;

            bounded_view() = default;
            explicit bounded_view(Rng rng)
              : rng_(detail::move(rng))
            {}
            iterator begin()
            {
                return iterator{ranges::begin(rng_)};
            }
            iterator end()
            {
                return iterator{end_(RA_and_Sized{})};
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_t<Rng> size()
            {
                return ranges::size(rng_);
            }
            CONCEPT_REQUIRES(View<Rng const>())
            iterator begin() const
            {
                return iterator{ranges::begin(rng_)};
            }
            CONCEPT_REQUIRES(View<Rng const>())
            iterator end() const
            {
                return iterator{ranges::end(rng_)};
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_t<Rng> size() const
            {
                return ranges::size(rng_);
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
            struct bounded_fn
            {
                template<typename Rng,
                    CONCEPT_REQUIRES_(Range<Rng>() && !BoundedRange<Rng>())>
                bounded_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return bounded_view<all_t<Rng>>{all(std::forward<Rng>(rng))};
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(Range<Rng>() && BoundedRange<Rng>())>
                all_t<Rng> operator()(Rng && rng) const
                {
                    return all(std::forward<Rng>(rng));
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Range<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(Range<Rng>(),
                        "Rng is not a model of the Range concept");
                }
            #endif
            };

            /// \relates bounded_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<bounded_fn>, bounded)

            template<typename Rng>
            using bounded_t =
                decltype(bounded(std::declval<Rng>()));
        }
        /// @}
    }
}

#endif
