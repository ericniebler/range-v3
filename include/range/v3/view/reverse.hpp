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

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <utility>
#include <iterator>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct reverse_view
          : view_adaptor<reverse_view<Rng>, Rng>
          , private detail::non_propagating_cache<
                iterator_t<Rng>, reverse_view<Rng>, !BoundedRange<Rng>()>
        {
        private:
            CONCEPT_ASSERT(BidirectionalRange<Rng>());
            friend range_access;

            // BoundedRange == true
            iterator_t<Rng> get_end_(std::true_type) const
            {
                return ranges::end(this->mutable_base());
            }
            // BoundedRange == false
            iterator_t<Rng> get_end_(std::false_type)
            {
                using cache_t = detail::non_propagating_cache<
                    iterator_t<Rng>, reverse_view<Rng>>;
                auto &end_ = static_cast<cache_t&>(*this);
                if(!end_)
                    end_ = ranges::next(
                        ranges::begin(this->mutable_base()),
                        ranges::end(this->mutable_base()));
                return *end_;
            }

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using reverse_view_t = meta::const_if_c<IsConst, reverse_view>;
#ifndef NDEBUG
                reverse_view_t *rng_;
#endif
            public:
                adaptor() = default;
                adaptor(reverse_view_t &rng)
#ifndef NDEBUG
                  : rng_(&rng)
#endif
                {
                    (void)rng;
                }
                static iterator_t<Rng> begin(reverse_view_t &rng)
                {
                    return rng.get_end_(BoundedRange<Rng>());
                }
                static iterator_t<Rng> end(reverse_view_t &rng)
                {
                    return ranges::begin(rng.base());
                }
                range_reference_t<Rng> read(iterator_t<Rng> it) const
                {
                    return *--it;
                }
                void next(iterator_t<Rng> &it) const
                {
                    RANGES_ASSERT(it != ranges::begin(rng_->base()));
                    --it;
                }
                void prev(iterator_t<Rng> &it) const
                {
                    RANGES_ASSERT(it != ranges::end(rng_->base()));
                    ++it;
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                void advance(iterator_t<Rng> &it, range_difference_type_t<Rng> n) const
                {
                    RANGES_ASSERT(n <= it - ranges::begin(rng_->base()));
                    RANGES_ASSERT(it - rng_->get_end_(BoundedRange<Rng>()) <= n);
                    ranges::advance(it, -n);
                }
                CONCEPT_REQUIRES(
                    SizedSentinel<iterator_t<Rng>, iterator_t<Rng>>())
                range_difference_type_t<Rng>
                distance_to(iterator_t<Rng> const &here, iterator_t<Rng> const &there,
                    adaptor const &other_adapt) const
                {
                    RANGES_ASSERT(rng_ == other_adapt.rng_); (void)other_adapt;
                    return here - there;
                }
            };
            adaptor<false> begin_adaptor()
            {
                return {*this};
            }
            adaptor<false> end_adaptor()
            {
                return {*this};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng const>())
            adaptor<true> begin_adaptor() const
            {
                return {*this};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng const>())
            adaptor<true> end_adaptor() const
            {
                return {*this};
            }
            // SizedRange == true
            range_size_type_t<Rng> size_(std::true_type)
            {
                return ranges::size(this->base());
            }
            // SizedRange == false, SizedSentinel == true
            range_size_type_t<Rng> size_(std::false_type)
            {
                // NB: This may trigger the O(N) walk over the sequence to find
                // last iterator. That cost is amortized over all calls to size()
                // and end, so we'll squint and call it "amortized O(1)."
                return ranges::iter_size(this->begin(), this->end());
            }
        public:
            reverse_view() = default;
            explicit reverse_view(Rng rng)
              : reverse_view::view_adaptor{std::move(rng)}
            {}
            CONCEPT_REQUIRES(!SizedRange<Rng const>() && (SizedRange<Rng>() ||
                SizedSentinel<iterator_t<Rng>, iterator_t<Rng>>()))
            range_size_type_t<Rng> size()
            {
                return this->size_(SizedRange<Rng>());
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_type_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct reverse_fn
            {
                template<typename Rng>
                using Concept = BidirectionalRange<Rng>;

                template<typename Rng, CONCEPT_REQUIRES_(Concept<Rng>())>
                reverse_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return reverse_view<all_t<Rng>>{all(static_cast<Rng&&>(rng))};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(BidirectionalRange<Rng>(),
                        "The object on which view::reverse operates must be a model of the "
                        "BidirectionalRange concept.");
                }
            #endif
            };

            /// \relates reverse_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<reverse_fn>, reverse)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::reverse_view)

#endif
