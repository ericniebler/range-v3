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
#include <iterator>
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/compressed_tuple.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange>
        struct stride_range_view : private range_base
        {
        private:
            using difference_type = range_difference_t<InputRange const>;
            InputRange rng_;
            difference_type stride_;

            static constexpr bool is_bidi()
            {
                return std::is_same<range_category_t<InputRange>,
                    std::bidirectional_iterator_tag>::value;
            }

            static constexpr bool is_rand()
            {
                return std::is_same<range_category_t<InputRange>,
                    std::random_access_iterator_tag>::value;
            }

            // Bidirectional stride iterators need a runtime boolean to keep track
            // of when the offset variable is dirty and needs to be recalculated.
            using is_dirty_t =
                detail::conditional_t<is_bidi(), mutable_<bool>, constant<bool, false>>;

            // Bidirectional and random-access stride iterators need to remember how
            // far past they end they are, so that when they're decremented, they can
            // visit the correct elements.
            using offset_t =
                detail::conditional_t<is_bidi() || is_rand(),
                                      mutable_<difference_type>,
                                      constant<difference_type, 0>>;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<InputRange>
                  , range_category_t<InputRange>
                  , range_reference_t<detail::add_const_if_t<InputRange, Const>>
                  , difference_type
                >
            {
            private:
                template<bool OtherConst>
                friend struct basic_iterator;
                friend struct stride_range_view;
                friend struct ranges::iterator_core_access;
                using stride_range_view_ = detail::add_const_if_t<stride_range_view, Const>;
                using base_range = detail::add_const_if_t<InputRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;

                stride_range_view_ *rng_;
                compressed_tuple<base_range_iterator, is_dirty_t, offset_t> it_dirt_off_;

                basic_iterator(stride_range_view_ &rng, begin_tag)
                  : rng_(&rng), it_dirt_off_{ranges::begin(rng_->rng_), false, 0}
                {}
                basic_iterator(stride_range_view_ &rng, end_tag)
                  : rng_(&rng), it_dirt_off_{ranges::end(rng_->rng_), true, 0}
                {
                    if(is_rand())
                        do_clean();
                }
                void increment()
                {
                    RANGES_ASSERT(it() != ranges::end(rng_->rng_));
                    RANGES_ASSERT(0 == offset());
                    this->set_offset(detail::advance_bounded(it(), rng_->stride_ + offset(),
                        ranges::end(rng_->rng_)));
                }
                template<bool OtherConst>
                bool equal(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it() == that.it();
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(it() != ranges::end(rng_->rng_));
                    RANGES_ASSERT(0 == offset());
                    return *it();
                }
                void decrement()
                {
                    RANGES_ASSERT(it() != ranges::begin(rng_->rng_));
                    clean();
                    this->set_offset(detail::advance_bounded(it(), -rng_->stride_ + offset(),
                        ranges::begin(rng_->rng_)));
                    RANGES_ASSERT(0 == offset());
                }
                template<bool OtherConst>
                difference_type distance_to(basic_iterator<OtherConst> const &that) const
                {
                    clean();
                    that.clean();
                    RANGES_ASSERT(rng_ == that.rng_);
                    RANGES_ASSERT(0 == ((that.it() - it()) +
                                        (that.offset() - offset())) % rng_->stride_);
                    return ((that.it() - it()) + (that.offset() - offset())) / rng_->stride_;
                }
                void advance(difference_type n)
                {
                    clean();
                    this->set_offset(detail::advance_bounded(it(), n * rng_->stride_ + offset(),
                        0 < n ? ranges::end(rng_->rng_) : ranges::begin(rng_->rng_)));
                }
                void clean() const
                {
                    if(is_dirty())
                    {
                        do_clean();
                        set_dirty(false);
                    }
                }
                base_range_iterator & it()
                {
                    return ranges::get<0>(it_dirt_off_);
                }
                base_range_iterator const & it() const
                {
                    return ranges::get<0>(it_dirt_off_);
                }
                void do_clean() const
                {
                    this->set_offset(ranges::distance(rng_->rng_) % rng_->stride_);
                    if(0 != offset())
                        this->set_offset(rng_->stride_ - offset());
                }
                bool is_dirty() const
                {
                    return ranges::get<1>(it_dirt_off_);
                }
                void set_dirty(bool b) const
                {
                    ranges::get<1>(it_dirt_off_) = b;
                }
                difference_type offset() const
                {
                    return ranges::get<2>(it_dirt_off_);
                }
                void set_offset(difference_type off) const
                {
                    ranges::get<2>(it_dirt_off_) = off;
                }
            public:
                constexpr basic_iterator()
                  : rng_{}, it_dirt_off_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst> = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : rng_(that.rng_), it_dirt_off_(std::move(that).it_dirt_off_)
                {}
            };
        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            stride_range_view(InputRange && rng, difference_type n)
              : rng_(std::forward<InputRange>(rng))
              , stride_(n)
            {
                RANGES_ASSERT(stride_ > 0);
            }
            iterator begin()
            {
                return {*this, begin_tag{}};
            }
            iterator end()
            {
                return {*this, end_tag{}};
            }
            const_iterator begin() const
            {
                return {*this, begin_tag{}};
            }
            const_iterator end() const
            {
                return {*this, end_tag{}};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            InputRange & base()
            {
                return rng_;
            }
            InputRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct strider : bindable<strider>
            {
                template<typename InputRange>
                static stride_range_view<InputRange>
                invoke(strider, InputRange && rng, range_difference_t<InputRange> step)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                    return {std::forward<InputRange>(rng), step};
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
