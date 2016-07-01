/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_STRIDE_HPP
#define RANGES_V3_VIEW_STRIDE_HPP

#include <atomic>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct stride_view
          : view_adaptor<
                stride_view<Rng>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            using size_type_ = range_size_t<Rng>;
            using difference_type_ = range_difference_t<Rng>;

            // Bidirectional and random-access stride iterators need to remember how
            // far past they end they are, so that when they're decremented, they can
            // visit the correct elements.
            using offset_t =
                meta::if_<
                    BidirectionalRange<Rng>,
                    mutable_<std::atomic<difference_type_>>,
                    constant<difference_type_, 0>>;

            difference_type_ stride_;

            struct adaptor : adaptor_base, private offset_t
            {
            private:
                using iterator = ranges::range_iterator_t<Rng>;
                stride_view const *rng_;
                offset_t & offset() { return *this; }
                offset_t const & offset() const { return *this; }
                difference_type_ clean_(std::true_type) const
                {
                    std::atomic<difference_type_>& off = offset();
                    difference_type_ o = off;
                    if(o == -1)
                    {
                        // Set the offset if it's still -1. If not, leave it alone.
                        (void) off.compare_exchange_strong(o, calc_offset());
                    }
                    return o;
                }
                difference_type_ clean_(std::false_type) const
                {
                    return 0;
                }
                difference_type_ clean() const
                {
                    return clean_(BidirectionalRange<Rng>());
                }
                difference_type_ calc_offset() const
                {
                    auto tmp = ranges::distance(rng_->base()) % rng_->stride_;
                    return 0 != tmp ? rng_->stride_ - tmp : tmp;
                }
            public:
                adaptor() = default;
                adaptor(stride_view const &rng, begin_tag)
                  : offset_t(0), rng_(&rng)
                {}
                adaptor(stride_view const &rng, end_tag)
                  : offset_t(-1), rng_(&rng)
                {
                    // Opportunistic eager cleaning when we can do so in O(1)
                    if(BidirectionalRange<Rng>() && SizedRange<Rng>())
                        offset() = calc_offset();
                }
                void next(iterator &it)
                {
                    difference_type_ off = offset();
                    RANGES_ASSERT(0 == off);
                    RANGES_ASSERT(it != ranges::end(rng_->mutable_base()));
                    offset() = ranges::advance(it, rng_->stride_ + off,
                        ranges::end(rng_->mutable_base()));
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng>())
                void prev(iterator &it)
                {
                    difference_type_ off = clean();
                    offset() = off = ranges::advance(it, -rng_->stride_ + off,
                        ranges::begin(rng_->mutable_base()));
                    RANGES_ASSERT(0 == off);
                }
                CONCEPT_REQUIRES(SizedSentinel<iterator, iterator>())
                difference_type_ distance_to(iterator here, iterator there, adaptor const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    difference_type_ delta = (there - here) + (that.clean() - clean());
                    if(BidirectionalIterator<iterator>())
                    {
                        RANGES_ASSERT(0 == delta % rng_->stride_);
                    }
                    else
                    {
                        delta += rng_->stride_ - 1;
                    }
                    return delta / rng_->stride_;
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                void advance(iterator &it, difference_type_ n)
                {
                    if(0 == n)
                        return;
                    difference_type_ off = clean();
                    if(0 < n)
                        offset() = ranges::advance(it, n * rng_->stride_ + off,
                            ranges::end(rng_->mutable_base()));
                    else if(0 > n)
                        offset() = ranges::advance(it, n * rng_->stride_ + off,
                            ranges::begin(rng_->mutable_base()));
                }
            };
            adaptor begin_adaptor() const
            {
                return {*this, begin_tag{}};
            }
            // If the underlying sequence object doesn't model BoundedRange, then we can't
            // decrement the end and there's no reason to adapt the sentinel. Strictly
            // speaking, we don't have to adapt the end iterator of Input and Forward
            // Ranges, but in the interests of making the resulting stride view model
            // BoundedView, adapt it anyway.
            CONCEPT_REQUIRES(!BoundedRange<Rng>())
            adaptor_base end_adaptor() const
            {
                return {};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng>())
            adaptor end_adaptor() const
            {
                return {*this, end_tag{}};
            }
        public:
            stride_view() = default;
            stride_view(Rng rng, difference_type_ stride)
              : stride_view::view_adaptor{std::move(rng)}
              , stride_(stride)
            {
                RANGES_ASSERT(0 < stride_);
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            size_type_ size() const
            {
                return (ranges::size(this->base()) + static_cast<size_type_>(stride_) - 1) /
                    static_cast<size_type_>(stride_);
            }
        };

        namespace view
        {
            struct stride_fn
            {
            private:
                friend view_access;
                template<typename Difference, CONCEPT_REQUIRES_(Integral<Difference>())>
                static auto bind(stride_fn stride, Difference step)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(stride, std::placeholders::_1, std::move(step)))
                )
            public:
                template<typename Rng, CONCEPT_REQUIRES_(InputRange<Rng>())>
                stride_view<all_t<Rng>> operator()(Rng && rng, range_difference_t<Rng> step) const
                {
                    return {all(std::forward<Rng>(rng)), step};
                }

                // For the purpose of better error messages:
            #ifndef RANGES_DOXYGEN_INVOKED
            private:
                template<typename Difference, CONCEPT_REQUIRES_(!Integral<Difference>())>
                static detail::null_pipe bind(stride_fn, const Difference &)
                {
                    CONCEPT_ASSERT_MSG(Integral<Difference>(),
                        "The value to be used as the step in a call to view::stride must be a "
                        "model of the Integral concept that is convertible to the range's "
                        "difference type.");
                    return {};
                }
            public:
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!InputRange<Rng>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be operated on by view::stride should be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The value to be used as the step in a call to view::stride must be a "
                        "model of the Integral concept that is convertible to the range's "
                        "difference type.");
                }
            #endif
            };

            /// \relates stride_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<stride_fn>, stride)
        }
        /// @}
    }
}

#endif
