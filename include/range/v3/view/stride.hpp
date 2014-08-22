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

#ifndef RANGES_V3_VIEW_STRIDE_HPP
#define RANGES_V3_VIEW_STRIDE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct strided_view
          : range_adaptor<strided_view<Rng>, Rng>
        {
        private:
            friend range_core_access;
            using size_type = range_size_t<Rng>;
            using difference_type = range_difference_t<Rng>;

            // Bidirectional stride iterators need a runtime boolean to keep track
            // of when the offset variable is dirty and needs to be lazily calculated.
            // Ditto for random-access stride iterators when the end is a sentinel.
            // If the size of the range is known a priori, then the runtime boolean
            // is always unnecessary.
            using dirty_t =
                detail::conditional_t<
                    (ConvertibleToBidirectionalRange<Rng>() && !ConvertibleToSizedRange<Rng>()),
                    mutable_<bool>,
                    constant<bool, false>>;

            // Bidirectional and random-access stride iterators need to remember how
            // far past they end they are, so that when they're decremented, they can
            // visit the correct elements.
            using offset_t =
                detail::conditional_t<
                    (ConvertibleToBidirectionalRange<Rng>()),
                    mutable_<difference_type>,
                    constant<difference_type, 0>>;

            difference_type stride_;

            struct adaptor : default_adaptor, private dirty_t, private offset_t
            {
            private:
                using base_cursor_t = ranges::base_cursor_t<strided_view>;
                using derived_cursor_t = ranges::derived_cursor_t<base_cursor_t, adaptor>;
                strided_view const *rng_;
                dirty_t & dirty() { return *this; }
                dirty_t const & dirty() const { return *this; }
                offset_t & offset() { return *this; }
                offset_t const & offset() const { return *this; }
                void clean() const
                {
                    // Possible race on dirty(), but it's harmless I think. Two threads
                    // might compute offset and set it independently, but the result should
                    // be the same.
                    if(dirty())
                        dirty() = (do_clean(), false);
                }
                void do_clean() const
                {
                    auto tmp = rng_->base_distance() % rng_->stride_;
                    offset() = 0 != tmp ? rng_->stride_ - tmp : tmp;
                }
            public:
                adaptor() = default;
                adaptor(strided_view const &rng, begin_tag)
                  : dirty_t(false), offset_t(0), rng_(&rng)
                {}
                adaptor(strided_view const &rng, end_tag)
                  : dirty_t(true), offset_t(0), rng_(&rng)
                {
                    // Opportunistic eager cleaning when we can do so in O(1)
                    if(ConvertibleToBidirectionalSizedRange<Rng>())
                        do_clean();
                }
                void next(base_cursor_t &pos)
                {
                    RANGES_ASSERT(0 == offset());
                    auto rng = as_iterator_pair(std::move(pos),
                        default_adaptor::end(*rng_));
                    RANGES_ASSERT(rng.first != rng.second);
                    offset() = advance_bounded(rng.first, rng_->stride_ + offset(),
                        rng.second);
                    pos = ranges::range_core_access::cursor(std::move(rng.first));
                }
                CONCEPT_REQUIRES(ConvertibleToBidirectionalRange<Rng>())
                void prev(base_cursor_t &pos)
                {
                    clean();
                    auto rng = as_iterator_pair(default_adaptor::begin(*rng_),
                        std::move(pos));
                    offset() = advance_bounded(rng.second, -rng_->stride_ + offset(),
                        rng.first);
                    RANGES_ASSERT(0 == offset());
                    pos = ranges::range_core_access::cursor(std::move(rng.second));
                }
                CONCEPT_REQUIRES(ConvertibleToRandomAccessRange<Rng>())
                difference_type distance_to(derived_cursor_t const &here,
                    derived_cursor_t const &there) const
                {
                    clean();
                    there.adaptor().clean();
                    RANGES_ASSERT(this == &here.adaptor());
                    RANGES_ASSERT(rng_ == there.adaptor().rng_);
                    RANGES_ASSERT(0 == (here.distance_to(there) +
                        (there.adaptor().offset() - offset())) % rng_->stride_);
                    return (here.distance_to(there) +
                        (there.adaptor().offset() - offset())) / rng_->stride_;
                }
                CONCEPT_REQUIRES(ConvertibleToRandomAccessRange<Rng>())
                void advance(base_cursor_t &pos, difference_type n)
                {
                    clean();
                    if(0 < n)
                    {
                        auto rng = as_iterator_pair(std::move(pos),
                            default_adaptor::end(*rng_));
                        offset() = advance_bounded(rng.first, n * rng_->stride_ + offset(),
                            rng.second);
                        pos = ranges::range_core_access::cursor(std::move(rng.first));
                    }
                    else if(0 > n)
                    {
                        auto rng = as_iterator_pair(default_adaptor::begin(*rng_),
                            std::move(pos));
                        offset() = advance_bounded(rng.second, n * rng_->stride_ + offset(),
                            rng.first);
                        pos = ranges::range_core_access::cursor(std::move(rng.second));
                    }
                }
            };
            // If the underlying sequence object doesn't model BoundedRange, then we can't
            // decrement the end and there's no reason to adapt the sentinel. Strictly
            // speaking, we don't have to adapt the end iterator of Input and Forward
            // Ranges, but in the interests of making the resulting stride view model
            // BoundedRange, adapt it anyway.
            auto end_adaptor_(concepts::ConvertibleToRange*) const -> default_adaptor
            {
                return {};
            }
            auto end_adaptor_(concepts::ConvertibleToBoundedRange*) const -> adaptor
            {
                return {*this, end_tag{}};
            }

            adaptor begin_adaptor() const
            {
                return {*this, begin_tag{}};
            }
            detail::conditional_t<(ConvertibleToBoundedRange<Rng>()), adaptor, default_adaptor>
            end_adaptor() const
            {
                return strided_view::end_adaptor_(convertible_to_bounded_range_concept<Rng>());
            }
        public:
            strided_view() = default;
            strided_view(Rng &&rng, difference_type stride)
              : range_adaptor_t<strided_view>{std::forward<Rng>(rng)}
              , stride_(stride)
            {
                RANGES_ASSERT(0 < stride_);
            }
            CONCEPT_REQUIRES(ConvertibleToSizedRange<Rng>())
            size_type size() const
            {
                return (this->base_size() + static_cast<size_type>(stride_) - 1) /
                    static_cast<size_type>(stride_);
            }
        };

        namespace view
        {
            struct stride_fn : bindable<stride_fn>
            {
                template<typename Rng>
                static strided_view<Rng>
                invoke(stride_fn, Rng && rng, range_difference_t<Rng> step)
                {
                    CONCEPT_ASSERT(ConvertibleToInputRange<Rng>());
                    return {std::forward<Rng>(rng), step};
                }

                template<typename Difference>
                static auto invoke(stride_fn stride, Difference step) ->
                    decltype(stride.move_bind(std::placeholders::_1, std::move(step)))
                {
                    CONCEPT_ASSERT(Integral<Difference>());
                    return stride.move_bind(std::placeholders::_1, std::move(step));
                }
            };

            RANGES_CONSTEXPR stride_fn stride{};
        }
    }
}

#endif
