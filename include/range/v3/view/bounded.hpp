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
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/common_iterator.hpp>
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
            friend range_access;
            using base_iterator_t = range_iterator_t<Rng>;
            using base_sentinel_t = range_sentinel_t<Rng>;
            Rng rng_;

        public:
            using iterator = common_iterator<base_iterator_t, base_sentinel_t>;

            bounded_view() = default;
            explicit bounded_view(Rng rng)
              : rng_(std::move(rng))
            {}
            iterator begin()
            {
                return iterator{ranges::begin(rng_)};
            }
            iterator end()
            {
                return iterator{ranges::end(rng_)};
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
            CONCEPT_REQUIRES(SizedView<Rng>())
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
            namespace
            {
                constexpr auto&& bounded = static_const<view<bounded_fn>>::value;
            }

            template<typename Rng>
            using bounded_t =
                decltype(bounded(std::declval<Rng>()));
        }
        /// @}
    }
}

#endif
