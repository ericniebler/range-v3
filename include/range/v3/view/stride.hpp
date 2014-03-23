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
#include <range/v3/next_prev.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable>
        struct stride_iterable_view
          : range_adaptor<stride_iterable_view<InputIterable>, InputIterable>
        {
        private:
            friend range_core_access;
            using size_type = range_size_t<InputIterable const>;
            using difference_type = range_difference_t<InputIterable const>;

            // Bidirectional stride iterators need a runtime boolean to keep track
            // of when the offset variable is dirty and needs to be lazily calculated.
            // Ditto for random-access stride iterators when the end is a sentinel.
            // If the size of the range is known a priori, then the runtime boolean
            // is always unnecessary.
            using dirty_t =
                detail::conditional_t<
                    (ranges::BidirectionalIterator<range_iterator_t<InputIterable>>() &&
                    !ranges::SizedIterable<InputIterable>()),
                    mutable_<bool>,
                    constant<bool, false>>;

            // Bidirectional and random-access stride iterators need to remember how
            // far past they end they are, so that when they're decremented, they can
            // visit the correct elements.
            using offset_t =
                detail::conditional_t<
                    (ranges::BidirectionalIterator<range_iterator_t<InputIterable>>()),
                    mutable_<difference_type>,
                    constant<difference_type, 0>>;

            difference_type stride_;

            struct range_adaptor : adaptor_defaults, private dirty_t, private offset_t
            {
            private:
                using base_cursor_t = base_cursor_t<stride_iterable_view>;
                stride_iterable_view const *rng_;
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
                    auto tmp = ranges::distance(rng_->base()) % rng_->stride_;
                    offset() = 0 != tmp ? rng_->stride_ - tmp : tmp;
                }
            public:
                range_adaptor() = default;
                range_adaptor(stride_iterable_view const &rng, begin_tag)
                  : dirty_t(false), offset_t(0), rng_(&rng)
                {}
                range_adaptor(stride_iterable_view const &rng, end_tag)
                  : dirty_t(true), offset_t(0), rng_(&rng)
                {
                    // Opportunistic eager cleaning when we can do so in O(1)
                    if(ranges::SizedIterable<InputIterable>() &&
                       ranges::BidirectionalIterator<range_iterator_t<InputIterable>>())
                        do_clean();
                }
                void next(base_cursor_t &pos)
                {
                    RANGES_ASSERT(0 == offset());
                    auto rng = ranges::as_iterator_pair(std::move(pos),
                        adaptor_defaults::end(*rng_));
                    RANGES_ASSERT(rng.first != rng.second);
                    offset() = detail::advance_bounded(rng.first, rng_->stride_ + offset(),
                        rng.second);
                    pos = ranges::range_core_access::cursor(std::move(rng.first));
                }
                CONCEPT_REQUIRES(ranges::BidirectionalIterator<range_iterator_t<InputIterable>>())
                void prev(base_cursor_t &pos)
                {
                    clean();
                    auto rng = ranges::as_iterator_pair(adaptor_defaults::begin(*rng_),
                        std::move(pos));
                    offset() = detail::advance_bounded(rng.second, -rng_->stride_ + offset(),
                        rng.first);
                    RANGES_ASSERT(0 == offset());
                    pos = ranges::range_core_access::cursor(std::move(rng.second));
                }
                template<typename Cursor,
                    CONCEPT_REQUIRES_(ranges::RandomAccessIterator<range_iterator_t<InputIterable>>())>
                difference_type distance_to(Cursor const &here,
                    Cursor const &there) const
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
                CONCEPT_REQUIRES(ranges::RandomAccessIterator<range_iterator_t<InputIterable>>())
                void advance(base_cursor_t &pos, difference_type n)
                {
                    clean();
                    if(0 < n)
                    {
                        auto rng = ranges::as_iterator_pair(std::move(pos),
                            adaptor_defaults::end(*rng_));
                        offset() = detail::advance_bounded(rng.first, n * rng_->stride_ + offset(),
                            rng.second);
                        pos = ranges::range_core_access::cursor(std::move(rng.first));
                    }
                    else if(0 > n)
                    {
                        auto rng = ranges::as_iterator_pair(adaptor_defaults::begin(*rng_),
                            std::move(pos));
                        offset() = detail::advance_bounded(rng.second, n * rng_->stride_ + offset(),
                            rng.first);
                        pos = ranges::range_core_access::cursor(std::move(rng.second));
                    }
                }
            };
            // If the underlying sequence object doesn't model Range, then we can't
            // decrement the end and there's no reason to adapt the sentinel. Strictly
            // speaking, we don't have to adapt the end iterator of Input and Forward
            // Ranges, but in the interests of making the resulting stride view model
            // Range, adapt it anyway.
            auto get_end_adaptor(concepts::Iterable) const -> adaptor_defaults
            {
                return {};
            }
            auto get_end_adaptor(concepts::Range) const -> range_adaptor
            {
                return {*this, end_tag{}};
            }

            range_adaptor get_adaptor(begin_tag) const
            {
                return {*this, begin_tag{}};
            }
            detail::conditional_t<(ranges::Range<InputIterable>()), range_adaptor, adaptor_defaults>
            get_adaptor(end_tag) const
            {
                return stride_iterable_view::get_end_adaptor(range_concept_t<InputIterable>{});
            }
        public:
            stride_iterable_view() = default;
            stride_iterable_view(InputIterable &&rng, difference_type stride)
              : range_adaptor_t<stride_iterable_view>{std::forward<InputIterable>(rng)}
              , stride_(stride)
            {
                RANGES_ASSERT(0 < stride_);
            }
            CONCEPT_REQUIRES(ranges::SizedIterable<InputIterable>())
            size_type size() const
            {
                return (this->base_size() + static_cast<size_type>(stride_) - 1) /
                    static_cast<size_type>(stride_);
            }
        };

        namespace view
        {
            struct strider : bindable<strider>
            {
                template<typename InputIterable>
                static stride_iterable_view<InputIterable>
                invoke(strider, InputIterable && rng, range_difference_t<InputIterable> step)
                {
                    CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                    return {std::forward<InputIterable>(rng), step};
                }

                template<typename Difference>
                static auto invoke(strider stride, Difference step) ->
                    decltype(stride.move_bind(std::placeholders::_1, std::move(step)))
                {
                    CONCEPT_ASSERT(ranges::Integral<Difference>());
                    return stride.move_bind(std::placeholders::_1, std::move(step));
                }
            };

            RANGES_CONSTEXPR strider stride{};
        }
    }
}

#endif
